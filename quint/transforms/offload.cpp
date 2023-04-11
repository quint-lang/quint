//
// Created by BY210033 on 2023/4/10.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/program/program.h"
#include "quint/analysis/analysis.h"
#include "quint/system/profiler.h"


#include <set>
#include <unordered_map>
#include <utility>

namespace quint::lang::irpass {

    namespace {

        class SquashPtrOffset : public IRVisitor {
        public:
            SquashPtrOffset() {
                allow_undefined_visitor = true;
                invoke_default_visitor = true;
            }

            void visit(Stmt *stmt) override {
                top_level_ptr = stmt;
            }

            static Stmt *run(Stmt *root) {
                SquashPtrOffset v;
                root->accept(&v);
                return v.top_level_ptr;
            }

        private:
            Stmt *top_level_ptr = nullptr;
        };

        using StmtToOffsetMap = std::unordered_map<const Stmt *, std::size_t>;

        struct OffloadedRanges {
            using Map = std::unordered_map<const OffloadedStmt *, Stmt *>;
            Map begin_stmts;
            Map end_stmts;
        };

        class Offloader {
        public:
            static OffloadedRanges run(IRNode *root, const CompileConfig &config) {
                OffloadedRanges offloaded_ranges;

                auto root_block = dynamic_cast<Block *>(root);
                auto root_statements = std::move(root_block->statements);
                root_block->statements.clear();
                auto pending_serial_statements =
                        Stmt::make_typed<OffloadedStmt>(OffloadedStmt::TaskType::serial);
                pending_serial_statements->grid_dim = 1;
                pending_serial_statements->block_dim = 1;

                auto assemble_serial_statements = [&]() {
                    if (!pending_serial_statements->body->statements.empty()) {
                        root_block->insert(std::move(pending_serial_statements));
                        pending_serial_statements =
                                Stmt::make_typed<OffloadedStmt>(OffloadedStmt::TaskType::serial);
                        pending_serial_statements->grid_dim = 1;
                        pending_serial_statements->block_dim = 1;
                    }
                };

                for (int i = 0; i < (int) root_statements.size(); ++i) {
                    auto &stmt = root_statements[i];
                    if (auto s = stmt->cast<RangeForStmt>(); s && !s->strictly_serialized) {
                        assemble_serial_statements();
                        auto offloaded = Stmt::make_typed<OffloadedStmt>(OffloadedStmt::TaskType::range_for);
                        offloaded->grid_dim = config.saturating_grid_dim;
                        if (s->block_dim == 0) {
                            offloaded->block_dim = Program::default_block_dim(config);
                        } else {
                            offloaded->block_dim = s->block_dim;
                        }
                        if (auto val = s->begin->cast<ConstStmt>()) {
                            offloaded->const_begin = true;
                            offloaded->begin_value = val->val.val_int32();
                        } else {
                            offloaded_ranges.begin_stmts.insert(
                                    std::make_pair(offloaded.get(), s->begin));
                        }

                        if (auto val = s->end->cast<ConstStmt>()) {
                            offloaded->const_end = true;
                            offloaded->end_value = val->val.val_int32();
                        } else {
                            // todo handler gpu and cuda
                            offloaded_ranges.end_stmts.insert(
                                    std::make_pair(offloaded.get(), s->end));
                        }

                        offloaded->num_cpu_threads = std::min(s->num_cpu_threads, config.cpu_max_num_threads);
                        replace_all_usages_with(s, s, offloaded.get());
                        for (int j = 0; j < (int) s->body->statements.size(); ++j) {
                            offloaded->body->insert(std::move(s->body->statements[j]));
                        }
                        offloaded->range_hint = s->range_hint;
                        root_block->insert(std::move(offloaded));
                    } else if (auto st = stmt->cast<StructForStmt>()) {
                        assemble_serial_statements();
                        emit_struct_for(st, root_block, config, st->mem_access_opt);
                    } else if (auto st = stmt->cast<MeshForStmt>()) {
                        assemble_serial_statements();
                        auto offloaded = Stmt::make_typed<OffloadedStmt>(OffloadedStmt::TaskType::mesh_for);
                        offloaded->grid_dim = config.saturating_grid_dim;
                        if (st->block_dim == 0) {
                            offloaded->block_dim = Program::default_block_dim(config);
                        } else {
                            offloaded->block_dim = st->block_dim;
                        }
                        offloaded->num_cpu_threads = std::min(st->num_cpu_threads, config.cpu_max_num_threads);
                        replace_all_usages_with(st, st, offloaded.get());
                        for (int j = 0; j < (int) st->body->statements.size(); ++j) {
                            offloaded->body->insert(std::move(st->body->statements[j]));
                        }
                        offloaded->mesh = st->mesh;
                        offloaded->major_from_type = std::move(st->major_from_type);
                        offloaded->major_to_types = std::move(st->major_to_types);
                        offloaded->minor_relation_types = std::move(st->minor_relation_types);
                        offloaded->mem_access_opt = st->mem_access_opt;
                        root_block->insert(std::move(offloaded));
                    } else {
                        pending_serial_statements->body->insert(std::move(stmt));
                    }
                }
                assemble_serial_statements();
                return offloaded_ranges;
            }

        private:
            static void emit_struct_for(StructForStmt *for_stmt,
                                        Block *root_block,
                                        const CompileConfig &config,
                                        const MemoryAccessOptions &mem_access_opt) {
                auto leaf = for_stmt->snode;

                std::vector<SNode *> path;

                for (auto p = leaf; p; p = p->parent) {
                    path.push_back(p);
                }
                std::reverse(path.begin(), path.end());

                const bool demotable =
                        (leaf->is_path_all_dense && config.demote_dense_struct_for);
                if (!demotable) {
                    for (int i = 1; i < path.size(); ++i) {
                        auto snode_child = path[i];
                        if (snode_child->type == SNodeType::quant_array &&
                            for_stmt->is_bit_vectorized) {
                            QUINT_ASSERT(i == path.size() - 1)
                            continue;
                        }
                        auto offloaded_clear_list = Stmt::make_typed<OffloadedStmt>(
                                OffloadedStmt::TaskType::serial);
                        offloaded_clear_list->body->insert(
                                Stmt::make_typed<ClearListStmt>(snode_child));
                        offloaded_clear_list->grid_dim = 1;
                        offloaded_clear_list->block_dim = 1;

                        root_block->insert(std::move(offloaded_clear_list));
                        auto offloaded_listgen = Stmt::make_typed<OffloadedStmt>(
                                OffloadedStmt::TaskType::serial);
                        offloaded_listgen->snode = snode_child;
                        offloaded_listgen->grid_dim = config.saturating_grid_dim;
                        offloaded_listgen->block_dim = std::min(snode_child->max_num_elements(),
                                                                (int64)std::min(Program::default_block_dim(config),
                                                                                config.max_block_dim));
                        root_block->insert(std::move(offloaded_listgen));
                    }
                }

                auto offloaded_struct_for = Stmt::make_typed<OffloadedStmt>(
                        OffloadedStmt::TaskType::struct_for);
                offloaded_struct_for->index_offsets = for_stmt->index_offsets;
                offloaded_struct_for->grid_dim = config.saturating_grid_dim;

                const auto snode_num_elements = for_stmt->snode->max_num_elements();
                if (for_stmt->block_dim == 0) {
                    offloaded_struct_for->block_dim =
                            std::min(snode_num_elements, (int64)config.default_gpu_block_dim);
                } else {
                    if (for_stmt->block_dim > snode_num_elements) {
                        QUINT_WARN(
                                "Specified block_dim {} is larger than the number of elements size {}."
                                "Clipping.\n{}.",
                                for_stmt->block_dim, snode_num_elements, for_stmt->tb);
                        offloaded_struct_for->block_dim = snode_num_elements;
                    } else {
                        offloaded_struct_for->block_dim = for_stmt->block_dim;
                    }
                }

                replace_all_usages_with(for_stmt, for_stmt, offloaded_struct_for.get());

                for (int i = 0; i < (int) for_stmt->body->statements.size(); ++i) {
                    offloaded_struct_for->body->insert(std::move(for_stmt->body->statements[i]));
                }

                offloaded_struct_for->snode = for_stmt->snode;
                offloaded_struct_for->is_bit_vectorized = for_stmt->is_bit_vectorized;
                offloaded_struct_for->num_cpu_threads = std::min(for_stmt->num_cpu_threads,
                                                                 config.cpu_max_num_threads);
                offloaded_struct_for->mem_access_opt = mem_access_opt;

                root_block->insert(std::move(offloaded_struct_for));
            }
        };

        class StmtToOffloaded : public BasicStmtVisitor {
        private:
            StmtToOffloaded() {
                current_offloaded_ = nullptr;
                allow_undefined_visitor = true;
                invoke_default_visitor = true;
            }

        public:
            void preprocess_container_stmt(Stmt *stmt) override {
                if (current_offloaded_ != nullptr)
                    stmt_to_offloaded_[stmt] = current_offloaded_;
            }

            void visit(Stmt *stmt) override {

            }

            static std::unordered_map<Stmt *, Stmt *> run(IRNode *root) {
                StmtToOffloaded stmt_to_offloaded;
                root->accept(&stmt_to_offloaded);
                return stmt_to_offloaded.stmt_to_offloaded_;
            }

        private:
            using BasicStmtVisitor::visit;

            std::unordered_map<Stmt *, Stmt *> stmt_to_offloaded_;

            Stmt *current_offloaded_;
        };

        class IdentifyValuesUsedInOtherOffloads : public BasicStmtVisitor {
            using BasicStmtVisitor::visit;

        private:
            IdentifyValuesUsedInOtherOffloads(
                    const CompileConfig &config,
                    const std::unordered_map<Stmt *, Stmt *> &stmt_to_offloaded,
                    OffloadedRanges *offloaded_ranges)
                    : config_(config),
                      stmt_to_offloaded_(stmt_to_offloaded),
                      offloaded_ranges_(offloaded_ranges) {
                allow_undefined_visitor = true;
                invoke_default_visitor = true;
                current_offloaded_ = nullptr;
                global_offset_ = 0;
            }

            std::size_t allocate_global(DataType type) {
                auto ret = global_offset_;
                std::size_t type_size = data_type_size(type);
                global_offset_ = ((global_offset_ + type_size - 1) / type_size) * type_size;
                ret = global_offset_;
                global_offset_ += type_size;

                QUINT_ASSERT(global_offset_ < quint_global_tmp_buffer_size)
                return ret;
            }

        public:
            void test_and_allocate(Stmt *stmt) {
                if (stmt == nullptr)
                    return;
                if (stmt_to_offloaded_[stmt] == current_offloaded_)
                    return;
                if (stmt->is<ConstStmt>())
                    return;
                auto top_level_ptr = SquashPtrOffset::run(stmt);

                if (top_level_ptr->is<GlobalPtrStmt>() || stmt->is<ExternalPtrStmt>() ||
                        (stmt->is<ArgLoadStmt>() && stmt->as<ArgLoadStmt>()->is_ptr)) {
                    return;
                }
                // todo handler gpu
                if (local_to_global_.find(top_level_ptr) == local_to_global_.end()) {
                    local_to_global_[top_level_ptr] = allocate_global(top_level_ptr->ret_type);
                }
            }

            void generic_visit(Stmt *stmt) {
                int n_op = stmt->num_operands();
                for (int i = 0; i < n_op; ++i) {
                    auto op = stmt->operand(i);
                    test_and_allocate(op);
                }
            }

            void preprocess_container_stmt(Stmt *stmt) override {
                generic_visit(stmt);
            }

            void visit(Stmt *stmt) override {
                generic_visit(stmt);
            }

            static StmtToOffsetMap run(
                    IRNode *root,
                    const CompileConfig &config,
                    const std::unordered_map<Stmt *, Stmt *> &stmt_to_offloaded,
                    OffloadedRanges *offloaded_ranges) {
                IdentifyValuesUsedInOtherOffloads pass(config, stmt_to_offloaded, offloaded_ranges);
                root->accept(&pass);
                return pass.local_to_global_;
            }


        private:
            CompileConfig config_;
            std::unordered_map<Stmt *, Stmt *> stmt_to_offloaded_;
            OffloadedRanges *const offloaded_ranges_;
            StmtToOffsetMap local_to_global_;
            Stmt *current_offloaded_;
            std::size_t global_offset_;
        };

        class PromoteIntermediateToGlobalTmp : public BasicStmtVisitor {
            using BasicStmtVisitor::visit;
        private:
            explicit PromoteIntermediateToGlobalTmp(
                    const StmtToOffsetMap &local_to_global_offset)
                    : local_to_global_offset_(local_to_global_offset) {
                allow_undefined_visitor = true;
                invoke_default_visitor = true;
            }

        public:
            void visit(Stmt *stmt) override {

            }

            static void run(IRNode *root, const StmtToOffsetMap &local_to_global_offset) {
                PromoteIntermediateToGlobalTmp pass(local_to_global_offset);
                root->accept(&pass);
            }

        private:
            StmtToOffsetMap local_to_global_offset_;
            std::set<Stmt *> stored_to_global_;
        };

        class FixCrossOffloadReferences : public BasicStmtVisitor {
            using BasicStmtVisitor::visit;

        private:
            FixCrossOffloadReferences(
                    const CompileConfig &config,
                    const StmtToOffsetMap &local_to_global_offset,
                    const std::unordered_map<Stmt *, Stmt *> &stmt_to_offloaded,
                    OffloadedRanges *offloaded_ranges)
                    : config_(config),
                      local_to_global_offset_(local_to_global_offset),
                      stmt_to_offloaded_(stmt_to_offloaded),
                      offloaded_ranges_(offloaded_ranges) {
                allow_undefined_visitor = true;
                invoke_default_visitor = true;
            }

        public:
            bool visit_operand(Stmt *stmt, int index) {
                QUINT_ASSERT(index >= 0 && index < stmt->num_operands())
                auto op = stmt->operand(index);
                if (op == nullptr)
                    return false;
                if (stmt_to_offloaded_[op] == stmt_to_offloaded_[op])
                    return false;

                auto offloaded = stmt_to_offloaded_[stmt];

                if (op->is<GlobalPtrStmt>()) {
                    auto copy = op->clone();
                    auto pcopy = copy.get();
                    copy->as<GlobalPtrStmt>()->activate = false;
                    stmt_to_offloaded_[copy.get()] = offloaded;
                    stmt->set_operand(index, copy.get());
                    stmt->insert_before_me(std::move(copy));
                    generic_visit(pcopy);
                    return true;
                }

                if (local_to_global_offset_.find(op) == local_to_global_offset_.end()) {
                    auto copy = op->clone();
                    auto pcopy = copy.get();
                    stmt_to_offloaded_[copy.get()] = offloaded;
                    stmt->set_operand(index, copy.get());
                    stmt->insert_before_me(std::move(copy));
                    generic_visit(pcopy);
                } else {
                    auto global_temporary = Stmt::make<GlobalTemporaryStmt>(
                            local_to_global_offset_[op], op->ret_type);
                    stmt_to_offloaded_[global_temporary.get()] = offloaded;
                    stmt->set_operand(index, global_temporary.get());
                    if (op->is<AllocaStmt>() || op->ret_type.is_pointer()) {
                        stmt->insert_before_me(std::move(global_temporary));
                    } else {
                        auto load = Stmt::make<GlobalLoadStmt>(global_temporary.get());
                        stmt_to_offloaded_[load.get()] = offloaded;
                        stmt->set_operand(index, load.get());
                        stmt->insert_before_me(std::move(global_temporary));
                        stmt->insert_before_me(std::move(load));
                    }
                }
                return true;
            }

            void generic_visit(Stmt *stmt) {
                int n_op = stmt->num_operands();
                for (int i = 0; i < n_op; ++i) {
                    visit_operand(stmt, i);
                }
            }

            void preprocess_container_stmt(Stmt *stmt) override {
                generic_visit(stmt);
            }

            void visit(Stmt *stmt) override {
                generic_visit(stmt);
            }

            static void run(
                    IRNode *root,
                    const CompileConfig &config,
                    const StmtToOffsetMap &local_to_global_offset,
                    const std::unordered_map<Stmt *, Stmt *> &stmt_to_offloaded,
                    OffloadedRanges *offloaded_ranges) {
                FixCrossOffloadReferences pass(config, local_to_global_offset, stmt_to_offloaded,
                                               offloaded_ranges);
                root->accept(&pass);
            }

        private:
            [[maybe_unused]] const CompileConfig &config_;
            StmtToOffsetMap local_to_global_offset_;
            std::unordered_map<Stmt *, Stmt *> stmt_to_offloaded_;
            OffloadedRanges *const offloaded_ranges_;
            std::unordered_map<Stmt *, DataType> local_to_global_vector_type_;
        };

        void insert_gc(IRNode *root, const CompileConfig &config) {
            auto *b = dynamic_cast<Block *>(root);
            QUINT_ASSERT(b)
            std::vector<std::pair<int, std::vector<SNode *>>> gc_statements;
            for (int i = 0; i < (int) b->statements.size(); ++i) {
                auto snodes = irpass::analysis::gather_deactivations(b->statements[i].get());
                gc_statements.emplace_back(
                        std::make_pair(i, std::vector<SNode *>(snodes.begin(), snodes.end())));
            }

            for (int i = (int) b->statements.size() - 1; i >= 0; i--) {
                auto snodes = gc_statements[i].second;
                for (auto *snode : snodes) {
                    if (is_gc_able(snode->type)) {
                        auto gc_task = Stmt::make_typed<OffloadedStmt>(
                                OffloadedStmt::TaskType::gc);
                        gc_task->snode = snode;
                        b->insert(std::move(gc_task), i + 1);
                    }
                }
            }
            if (!irpass::analysis::gather_statements(root, [](Stmt *stmt) {
                return stmt->is<FuncCallStmt>();
            }).empty()) {
                auto gc_root = Stmt::make_typed<OffloadedStmt>(OffloadedStmt::TaskType::gc);
                b->insert(std::move(gc_root));
            }
        }

        class AssociateContinueScope : public BasicStmtVisitor {
        public:
            using BasicStmtVisitor::visit;
            using Parent = BasicStmtVisitor;

            static void run(IRNode *root) {
                while (true) {
                    AssociateContinueScope pass;
                    root->accept(&pass);
                    if (!pass.modified_)
                        break;
                }
            }

        private:
            explicit AssociateContinueScope()
                : modified_(false),
                  current_offloaded_stmt_(nullptr),
                  current_internal_loop_(nullptr) {
                allow_undefined_visitor = true;
                invoke_default_visitor = true;
            }

            bool modified_;
            OffloadedStmt *current_offloaded_stmt_;
            Stmt *current_internal_loop_;

        };

    }

    void offload(IRNode *root, const CompileConfig &config) {
        QUINT_AUTO_PROF;
        auto offloaded_ranges = Offloader::run(root, config);
        type_check(root, config);
        {
            auto stmt_to_offloaded = StmtToOffloaded::run(root);
            const auto local_to_global_offset = IdentifyValuesUsedInOtherOffloads::run(
                    root, config, stmt_to_offloaded, &offloaded_ranges);
            PromoteIntermediateToGlobalTmp::run(root, local_to_global_offset);
            stmt_to_offloaded = StmtToOffloaded::run(root);
            FixCrossOffloadReferences::run(root, config, local_to_global_offset, stmt_to_offloaded,
                                           &offloaded_ranges);
        }
        insert_gc(root, config);
        AssociateContinueScope::run(root);
        type_check(root, config);
        re_id(root);
    }

}
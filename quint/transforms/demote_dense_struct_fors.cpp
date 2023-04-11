//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/inc/constant.h"
#include "quint/transforms/utils.h"
#include "quint/util/bit.h"

namespace quint::lang {

    using TaskType = OffloadedStmt::TaskType;

    void convert_range_for(OffloadedStmt *offloaded, bool packaed) {
        QUINT_ASSERT(offloaded->task_type == TaskType::struct_for);

        std::vector<SNode *> snodes;
        auto *snode = offloaded->snode;
        int total_bits = 0;
        int start_bits[quint_max_num_indices] = {0};
        while (snode->type != SNodeType::root) {
            snodes.push_back(snode);
            for (int j = 0; j < quint_max_num_indices; j++) {
                start_bits[j] += snode->extractors[j].num_bits;
            }
            total_bits += snode->total_num_bits;
            snode = snode->parent;
        }
        std::reverse(snodes.begin(), snodes.end());

        int64 total_n = 1;
        std::array<int, quint_max_num_indices> total_shape;
        total_shape.fill(1);
        for (const auto *s : snodes) {
            for (int j = 0; j < quint_max_num_indices; ++j) {
                total_shape[j] *= s->extractors[j].shape;
            }
            total_n *= s->num_cells_per_container;
        }
        QUINT_ASSERT(total_n <= std::numeric_limits<int>::max());

        offloaded->const_begin = true;
        offloaded->const_end = true;
        offloaded->begin_value = 0;
        offloaded->end_value = total_n;

        auto body = std::move(offloaded->body);
        const int num_loop_vars = snodes.empty() ? 0 : snodes.back()->num_active_indices;

        std::vector<Stmt *> new_loop_vars;

        VecStatement body_header;

        std::vector<int> physical_indices;

        for (int i = 0; i < num_loop_vars; i++) {
            new_loop_vars.push_back(body_header.push_back<ConstStmt>(TypedConstant(0)));
            physical_indices.push_back(snodes.back()->physical_index_position[i]);
        }

        auto main_loop_var = body_header.push_back<LoopIndexStmt>(nullptr, 0);

        Stmt *test = body_header.push_back<ConstStmt>(TypedConstant(-1));
        bool has_test = false;
        if (packaed) {
            for (int i = 0; i < (int) snodes.size(); ++i) {
                auto *snode = snodes[i];
                Stmt *extracted = main_loop_var;
                if (i != 0) {
                    extracted = generate_mod(&body_header, extracted, total_n);
                }
                total_n /= snode->num_cells_per_container;
                extracted = generate_div(&body_header, extracted, total_n);
                bool is_first_extraction = true;
                for (int j = 0; j < (int) physical_indices.size(); ++j) {
                    auto p = physical_indices[j];
                    auto ext = snode->extractors[p];
                    if (!ext.active)
                        continue;
                    Stmt *index = extracted;
                    if (is_first_extraction) {
                        is_first_extraction = false;
                    } else {
                        index = generate_mod(&body_header, index, ext.acc_shape * ext.shape);
                    }
                    index = generate_div(&body_header, index, ext.acc_shape);
                    total_shape[p] /= ext.shape;
                    auto multiplier =
                            body_header.push_back<ConstStmt>(TypedConstant(total_shape[p]));
                    auto delta = body_header.push_back<BinaryOpStmt>(BinaryOpType::mul,
                                                                     index, multiplier);
                    new_loop_vars[j] = body_header.push_back<BinaryOpStmt>(BinaryOpType::add,
                                                                           new_loop_vars[j], delta);
                }
            }
        } else {
            int offset = total_bits;
            for (int i = 0; i < (int) snodes.size(); ++i) {
                auto snode = snodes[i];
                offset -= snode->total_num_bits;
                for (int j = 0; j < (int) physical_indices.size(); ++j) {
                    auto p = physical_indices[j];
                    auto ext = snode->extractors[p];
                    Stmt *delta = body_header.push_back<BitExtractStmt>(main_loop_var, ext.acc_offset + offset,
                                                                        ext.num_bits + offset + ext.acc_offset);
                    start_bits[p] -= ext.num_bits;
                    auto multiplier = body_header.push_back<ConstStmt>(TypedConstant(1 << start_bits[p]));
                    delta = body_header.push_back<BinaryOpStmt>(BinaryOpType::mul, delta, multiplier);
                    new_loop_vars[j] = body_header.push_back<BinaryOpStmt>(BinaryOpType::add,
                                                                           new_loop_vars[j], delta);
                }
            }

            if (!snodes.empty()) {
                auto snode = snodes.back();
                for (int j = 0; j < (int) physical_indices.size(); ++j) {
                    auto p = physical_indices[j];
                    auto num_elements = snode->extractors[p].num_elements_from_root;
                    if (!bit::is_power_of_two(num_elements)) {
                        has_test = true;
                        auto bound =
                                body_header.push_back<ConstStmt>(TypedConstant(num_elements));
                        auto cond = body_header.push_back<BinaryOpStmt>(BinaryOpType::cmp_lt,
                                                                        new_loop_vars[j], bound);
                        test = body_header.push_back<BinaryOpStmt>(BinaryOpType::bit_and, test, cond);
                    }
                }
            }
        }

        irpass::replace_statements(
                body.get(),
                [&](Stmt *s) {
                    if (auto loop_index = s->cast<LoopIndexStmt>()) {
                        return loop_index->loop == offloaded;
                    } else {
                        return false;
                    }
                },
                [&](Stmt *s) {
                    auto index = std::find(physical_indices.begin(), physical_indices.end(),
                                           s->as<LoopIndexStmt>()->index);
                    QUINT_ASSERT(index != physical_indices.end())
                    return new_loop_vars[index - physical_indices.begin()];
                });

        if (has_test) {
            auto if_stmt = Stmt::make_typed<IfStmt>(test);
            if_stmt->set_true_statements(std::move(body));
            body = std::make_unique<Block>();
            body->insert(std::move(if_stmt));
        }
        body->insert(std::move(body_header), 0);

        offloaded->body = std::move(body);
        offloaded->body->parent_stmt = offloaded;
        main_loop_var->loop = offloaded;

        offloaded->task_type = TaskType::range_for;
    }

    void maybe_convert(OffloadedStmt *stmt, bool packed) {
        if ((stmt->task_type == TaskType::struct_for) && stmt->snode->is_path_all_dense) {
            convert_range_for(stmt, packed);
        }
    }

    namespace irpass {

        void demote_dense_struct_fors(IRNode *root, bool packed) {
            if (auto *block = root->cast<Block>()) {
                for (auto &s_ : block->statements) {
                    if (auto *s = s_->cast<OffloadedStmt>()) {
                        maybe_convert(s, packed);
                    }
                }
            } else if (auto *stmt = root->cast<OffloadedStmt>()) {
                maybe_convert(stmt, packed);
            }
            re_id(root);
        }

    }

}

//
// Created by BY210033 on 2023/4/10.
//
#include <variant>

#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/program/program.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    class Scalarize : public BasicStmtVisitor {
    public:
        ImmediateIRModifier immediate_modifier_;
        DelayedIRModifier delayed_modifier_;

        explicit Scalarize(IRNode *node) : immediate_modifier_(node) {
            node->accept(this);

            delayed_modifier_.modify_ir();
        }

    private:
        using BasicStmtVisitor::visit;
    };

    class GatherScalarizablePointers : public BasicStmtVisitor {
    private:
        using BasicStmtVisitor::visit;

        std::unordered_map<Stmt *, bool> is_alloca_scalarizable_;

    public:

        static std::unordered_set<Stmt *> run(IRNode *node) {
            GatherScalarizablePointers pass;
            node->accept(&pass);
            std::unordered_set<Stmt *> result;
            for (auto &[k, v] : pass.is_alloca_scalarizable_) {
                if (v) {
                    result.insert(k);
                }
            }
            return result;
        }
    };

    class ScalarizeLocalPointers : public BasicStmtVisitor {
    public:
        ImmediateIRModifier immediate_modifier_;
        DelayedIRModifier delayed_modifier_;

        std::unordered_set<Stmt *> scalarizable_allocas_;
        std::unordered_map<Stmt *, std::vector<Stmt *>> scalarized_local_tensor_map_;

        explicit ScalarizeLocalPointers(
                IRNode *node,
                const std::unordered_set<Stmt *> &scalarizable_allocas)
                 : immediate_modifier_(node), scalarizable_allocas_(scalarizable_allocas) {
            node->accept(this);

            delayed_modifier_.modify_ir();
        }

    private:
        using BasicStmtVisitor::visit;
    };

    class ExtractLocalPointers : public BasicStmtVisitor {
    public:
        ImmediateIRModifier immediate_modifier_;
        DelayedIRModifier delayed_modifier_;

        std::unordered_map<std::pair<Stmt *, int>,
                            Stmt *,
                            hashing::Hasher<std::pair<Stmt *, int>>>
                            first_matrix_ptr_;

        std::unordered_map<int, Stmt *>
            first_const_;

        Block *top_level_;

        explicit ExtractLocalPointers(IRNode *root) : immediate_modifier_(root) {
            QUINT_ASSERT(root->is<Block>())
            top_level_ = root->as<Block>();
            root->accept(this);
            delayed_modifier_.modify_ir();
        }

    private:
        using BasicStmtVisitor::visit;
    };

    namespace irpass {
        void scalarize(IRNode *root, const CompileConfig &config) {
            QUINT_AUTO_PROF
            Scalarize pass(root);
            auto sclarizable_allocs = GatherScalarizablePointers::run(root);
            ScalarizeLocalPointers pass2(root, sclarizable_allocs);
            ExtractLocalPointers pass3(root);
        }
    }

}
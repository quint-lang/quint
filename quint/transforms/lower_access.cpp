//
// Created by BY210033 on 2023/4/11.
//
#include "quint/transforms/lower_access.h"
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/system/profiler.h"
#include "quint/program/program.h"
#include "quint/program/kernel.h"
#include "quint/transforms/scalar_pointer_lowerer.h"

#include <deque>

namespace quint::lang {

    class LowerAccess;

    class PtrLowerImpl : public ScalarPointerLowerer {
    public:
        using ScalarPointerLowerer::ScalarPointerLowerer;

    private:
        LowerAccess *la_{nullptr};
        std::unordered_set<SNode *> snodes_on_loop_;
        bool pointer_needs_activation_{false};
    };

    class LowerAccess : public IRVisitor {
    public:
        DelayedIRModifier modifier;
        StructForStmt *current_struct_for;
        const std::vector<SNode *> &kernel_forces_no_activate;
        bool lower_atomic_ptr;
        bool packed;

        LowerAccess(const std::vector<SNode *> &kernel_forces_no_activate,
                    bool lower_atomic_ptr,
                    bool packed)
                : kernel_forces_no_activate(kernel_forces_no_activate),
                  lower_atomic_ptr(lower_atomic_ptr),
                  packed(packed) {
            allow_undefined_visitor = true;
            current_struct_for = nullptr;
        }

        void visit(Block *stmt) override {

        }

        static bool run(IRNode *node, const std::vector<SNode *> &kernel_forces_no_activate,
                        bool lower_atomic, bool packed) {
            LowerAccess lower_access(kernel_forces_no_activate, lower_atomic, packed);
            bool modified = false;
            while (true) {
                node->accept(&lower_access);
                if (lower_access.modifier.modify_ir()) {
                    modified = true;
                } else {
                    break;
                }
            }
            return modified;
        }
    };

    const PassID LowerAccessPass::id = "LowerAccessPass";

    namespace irpass {

        bool lower_access(IRNode *node,
                          const CompileConfig &config,
                          const LowerAccessPass::Args &args) {
            bool modified = LowerAccess::run(node, args.kernel_forces_no_activate,
                                             args.lower_atomic, config.packed);
            type_check(node, config);
            return modified;
        }

    }

}

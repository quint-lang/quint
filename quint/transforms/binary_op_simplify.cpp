//
// Created by BY210033 on 2023/4/11.
//
#include "quint/analysis/analysis.h"
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    class BinaryOpSimp : public BasicStmtVisitor {
    public:
        using BasicStmtVisitor::visit;
        bool fast_math;
        DelayedIRModifier modifier;
        bool operand_swapped;

        explicit BinaryOpSimp(bool fast_math_)
            : fast_math(fast_math_), operand_swapped(false) {
        }

        static bool run(IRNode *node, bool fast_math) {
            BinaryOpSimp simp(fast_math);
            bool modified = false;
            while (true) {
                node->accept(&simp);
                if (simp.modifier.modify_ir()) {
                    modified = true;
                } else {
                    break;
                }
            }
            return modified || simp.operand_swapped;
        }
    };

    namespace irpass {

        bool binary_op_simplify(IRNode *node, const CompileConfig &config) {
            QUINT_AUTO_PROF
            return BinaryOpSimp::run(node, config.fast_math);
        }
    }

}

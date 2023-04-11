//
// Created by BY210033 on 2023/4/11.
//
#include "quint/analysis/analysis.h"
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/system/profiler.h"
#include "quint/program/program.h"

namespace quint::lang {

    class AlgSimp : public BasicStmtVisitor {
    private:

    public:
        static constexpr int max_weaken_exponent = 32;
        using BasicStmtVisitor::visit;
        bool fast_math;
        DelayedIRModifier modifier;

        explicit AlgSimp(bool fast_math_)
            : fast_math(fast_math_) {
        }

        static bool run(IRNode *node, bool fast_math) {
            AlgSimp simp(fast_math);
            bool modified = false;
            while (true) {
                node->accept(&simp);
                if (simp.modifier.modify_ir()) {
                    modified = true;
                } else {
                    break;
                }
            }
            return modified;
        }
    };

    namespace irpass {

        bool alg_simp(IRNode *node, const CompileConfig &config) {
            QUINT_AUTO_PROF
            return AlgSimp::run(node, config.fast_math);
        }
    }

}

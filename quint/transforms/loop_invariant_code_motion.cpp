//
// Created by BY210033 on 2023/4/11.
//
#include "quint/transforms/loop_invariant_detector.h"

namespace quint::lang {

    class LoopInvariantCodeMotion : public LoopInvariantDetector {
    public:
        using LoopInvariantDetector::visit;

        DelayedIRModifier modifier;

        explicit LoopInvariantCodeMotion(const CompileConfig &config)
                : LoopInvariantDetector(config) {
        }

        static bool run(IRNode *node, const CompileConfig &config) {
            bool modified = false;
            while (true) {
                LoopInvariantCodeMotion motion(config);
                node->accept(&motion);
                if (motion.modifier.modify_ir()) {
                    modified = true;
                } else {
                    break;
                }
            }
            return modified;
        }
    };

    namespace irpass {

        bool loop_invariant_code_motion(IRNode *node, const CompileConfig &config) {
            QUINT_AUTO_PROF
            return LoopInvariantCodeMotion::run(node, config);
        }
    }

}

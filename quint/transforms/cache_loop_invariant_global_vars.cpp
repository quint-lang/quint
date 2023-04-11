//
// Created by BY210033 on 2023/4/10.
//
#include "quint/transforms/loop_invariant_detector.h"
#include "quint/analysis/analysis.h"

namespace quint::lang {

    class CacheLoopInvariantGlobalVars : public LoopInvariantDetector {
    public:
        using LoopInvariantDetector::visit;

        enum class CacheStatus {
            None = 0,
            Read = 1,
            Write = 2,
            ReadWrite = 3,
        };

        typedef std::unordered_map<Stmt *, std::pair<CacheStatus, AllocaStmt *>>
            CacheMap;
        std::stack<CacheMap> cached_maps;

        DelayedIRModifier modifier;
        std::unordered_map<const SNode *, GlobalPtrStmt *> loop_unique_ptr_;
        std::unordered_map<int, ExternalPtrStmt *> loop_unique_arr_ptr;

        OffloadedStmt *current_offloaded;

        explicit CacheLoopInvariantGlobalVars(const CompileConfig &config)
            : LoopInvariantDetector(config) {
        }

        static bool run(IRNode *node, const CompileConfig &config) {
            bool modified = false;

            while (true) {
                CacheLoopInvariantGlobalVars eliminator(config);
                node->accept(&eliminator);
                if (eliminator.modifier.modify_ir())
                    modified = true;
                else
                    break;
            }

            return modified;
        }

    };

    namespace irpass {
        bool cache_loop_invariant_global_vars(IRNode *root,
                                              const CompileConfig &config) {
            QUINT_AUTO_PROF
            return CacheLoopInvariantGlobalVars::run(root, config);
        }
    }

}


//
// Created by BY210033 on 2023/4/11.
//
#include <deque>
#include <thread>

#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/ir/visitors.h"
#include "quint/program/program.h"
#include "quint/ir/snode.h"
#include "quint/transforms/constant_fold.h"
#include "quint/system/profiler.h"

namespace quint::lang {

    class ConstantFold : public BasicStmtVisitor {
    public:
        using BasicStmtVisitor::visit;
        DelayedIRModifier modifier;
        Program *program;

        explicit ConstantFold(Program *program_) : program(program_) {
        }

        static bool run(IRNode *node, Program *program) {
            ConstantFold constant_fold(program);
            bool modified = false;

            auto program_compile_config_org = program->this_thread_config();
            program->this_thread_config().advanced_optimization = false;
            program->this_thread_config().constant_folding = false;
            program->this_thread_config().external_optimization_level = 0;

            while (true) {
                node->accept(&constant_fold);
                if (constant_fold.modifier.modify_ir()) {
                    modified = true;
                } else {
                    break;
                }
            }

            program->this_thread_config() = program_compile_config_org;

            return modified;
        }
    };

    const PassID ConstantFoldPass::id = "ConstantFoldPass";

    namespace irpass {

        bool constant_fold(IRNode *root,
                        const CompileConfig &config,
                        const ConstantFoldPass::Args &args) {
            QUINT_AUTO_PROF
            if (!config.advanced_optimization)
                return false;
            return ConstantFold::run(root, args.program);
        }
    }

}

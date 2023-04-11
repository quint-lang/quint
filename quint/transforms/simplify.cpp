//
// Created by BY210033 on 2023/4/11.
//
#include "quint/ir/ir.h"
#include "quint/ir/statements.h"
#include "quint/ir/transforms.h"
#include "quint/analysis/analysis.h"
#include "quint/ir/visitors.h"
#include "quint/program/kernel.h"
#include "quint/program/program.h"
#include "quint/transforms/simplify.h"
#include "quint/system/profiler.h"

#include <set>
#include <unordered_set>
#include <utility>

namespace quint::lang {

    class BasicBlockSimplify : public IRVisitor {
    public:
        Block *block;

        int current_stmt_id;
        std::set<int> &visited;
        StructForStmt *current_struct_for;
        CompileConfig config;
        DelayedIRModifier modifier;

        BasicBlockSimplify(Block *block,
                           std::set<int> &visited,
                           StructForStmt *current_struct_for,
                           const CompileConfig &config)
                :   block(block),
                    visited(visited),
                    current_struct_for(current_struct_for),
                    config(config) {
            allow_undefined_visitor = true;
            invoke_default_visitor = true;
        }

        void accept_block() {
            for (int i = 0; i < (int) block->statements.size(); i++) {
                current_stmt_id = i;
                block->statements[i]->accept(this);
            }
        }

        static bool run(Block *block,
                        std::set<int> &visited,
                        StructForStmt *current_struct_for,
                        const CompileConfig &config) {
            BasicBlockSimplify simplify(block, visited, current_struct_for, config);
            bool ir_modified = false;
            while (true) {
                simplify.accept_block();
                if (simplify.modifier.modify_ir()) {
                    ir_modified = true;
                } else {
                    break;
                }
            }
            return ir_modified;
        }

    };

    class Simplify : public IRVisitor {
    public:
        StructForStmt *current_struct_for;
        bool modified;
        const CompileConfig &config;

        Simplify(IRNode *root, const CompileConfig &config)
                : config(config) {
            modified = false;
            current_struct_for = nullptr;
            allow_undefined_visitor = true;
            invoke_default_visitor = true;
            root->accept(this);
        }

        void visit(Block *stmt) override {

        }



    };

    const PassID FullSimplifyPass::id = "FullSimplifyPass";

    namespace irpass {

        bool simplify(IRNode *root, const CompileConfig &config) {
            QUINT_AUTO_PROF
            bool ir_modified = false;
            while (true) {
                Simplify pass(root, config);
                if (pass.modified)
                    ir_modified = true;
                else
                    break;
            }
            return ir_modified;
        }

        void full_simplify(IRNode *root,
                           const CompileConfig &config,
                           const FullSimplifyPass::Args &args) {
            QUINT_AUTO_PROF
            if (config.advanced_optimization) {
                bool first_iteration = true;
                while (true) {
                    bool ir_modified = false;
                    if (extract_constant(root, config))
                        ir_modified = true;
                    if (unreachable_code_elimination(root))
                        ir_modified = true;
                    if (binary_op_simplify(root, config))
                        ir_modified = true;
                    if (config.constant_folding &&
                        constant_fold(root, config, {args.program}))
                        ir_modified = true;
                    if (die(root))
                        ir_modified = true;
                    if (alg_simp(root, config))
                        ir_modified = true;
                    if (loop_invariant_code_motion(root, config))
                        ir_modified = true;
                    if (die(root))
                        ir_modified = true;
                    if (simplify(root, config))
                        ir_modified = true;
                    if (die(root))
                        ir_modified = true;
                    if (config.opt_level > 0 && whole_kernel_cse(root))
                        ir_modified = true;
                    if (config.opt_level > 0 && first_iteration && config.cfg_optimization &&
                        cfg_optimization(root, args.after_lower_access, args.autodiff_enabled,
                                         !config.real_matrix_scalarize))
                        ir_modified = true;
                    first_iteration = false;
                    if (!ir_modified)
                        break;
                }
                return;
            }
            if (config.constant_folding) {
                constant_fold(root, config, {args.program});
                die(root);
            }
            simplify(root, config);
            die(root);
        }

    }

}

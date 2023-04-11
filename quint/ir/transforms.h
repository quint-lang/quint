//
// Created by BY210033 on 2023/3/31.
//

#ifndef QUINT_TRANSFORMS_H
#define QUINT_TRANSFORMS_H

#include <atomic>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "quint/ir/ir.h"
#include "quint/program/kernel.h"
#include "quint/transforms/simplify.h"
#include "quint/transforms/constant_fold.h"
#include "quint/ir/control_flow_graph.h"
#include "quint/transforms/check_out_of_bound.h"
#include "quint/transforms/make_block_local.h"
#include "quint/transforms/lower_access.h"

namespace quint::lang {
    namespace irpass {

        void print(IRNode* root, std::string *output = nullptr);
        void fronted_type_check(IRNode *root);
        void lower_ast(IRNode *root);
        void re_id(IRNode *root);
        void flag_access(IRNode *root);
        bool die(IRNode *root);
        bool simplify(IRNode *root, const CompileConfig &config);
        bool cfg_optimization(IRNode *root,
                              bool after_lower_access,
                              bool autodiff_enabled,
                              bool real_matrix_enabled,
                              const std::optional<ControlFlowGraph::LiveVarAnalysisConfig>
                                      &lva_config_opt = std::nullopt);
        bool alg_simp(IRNode *root, const CompileConfig &config);
        void eliminate_immutable_local_vars(IRNode *root);
        void scalarize(IRNode *root, const CompileConfig &config);
        void type_check(IRNode *root, const CompileConfig &config);
        bool demote_operations(IRNode *root, const CompileConfig &config);
        bool binary_op_simplify(IRNode *root, const CompileConfig &config);
        bool whole_kernel_cse(IRNode *root);
        bool extract_constant(IRNode *root, const CompileConfig &config);
        bool unreachable_code_elimination(IRNode *root);
        bool loop_invariant_code_motion(IRNode *root, const CompileConfig &config);
        bool cache_loop_invariant_global_vars(IRNode *root,
                                              const CompileConfig &config);

        void full_simplify(IRNode *root,
                           const CompileConfig &config,
                           const FullSimplifyPass::Args &args);
        void bit_loop_vectorize(IRNode *root);
        void replace_all_usages_with(IRNode *root, Stmt *old, Stmt *new_ir);
        bool check_out_of_bound(IRNode *root,
                                const CompileConfig &config,
                                const CheckOutOfBoundPass::Args &args);
        void make_thread_local(IRNode *root, const CompileConfig &config);
        void make_block_local(IRNode *root, const CompileConfig &config, const MakeBlockLocalPass::Args &args);
        bool remove_loop_unique(IRNode *root);
        bool remove_range_assumption(IRNode *root);
        bool lower_access(IRNode *root,
                          const CompileConfig &config,
                          const LowerAccessPass::Args &args);

        bool demote_atomics(IRNode *root, const CompileConfig &config);
        void detect_read_only(IRNode *root);

        bool determine_ad_stack_size(IRNode *root, const CompileConfig &config);
        bool constant_fold(IRNode *root,
                           const CompileConfig &config,
                           const ConstantFoldPass::Args &args);

        void offload(IRNode *root, const CompileConfig &config);
        bool transform_statements(IRNode *root,
                                  std::function<bool(Stmt *)> filter,
                                  std::function<void(Stmt *, DelayedIRModifier *)> transformer);

        bool replace_statements(IRNode *root,
                                std::function<bool(Stmt *)> filter,
                                std::function<Stmt *(Stmt *)> finder);
        void demote_dense_struct_fors(IRNode *root, bool packed);

        void compile_to_offloads(IRNode *ir,
                                 const CompileConfig &config,
                                 Kernel *kernel,
                                 bool verbose,
                                 bool ad_use_stack,
                                 bool start_from_ast);

        void offload_to_executable(IRNode *ir,
                                   const CompileConfig &config,
                                   Kernel *kernel,
                                   bool verbose,
                                   bool determine_ad_stack_size,
                                   bool lower_global_access,
                                   bool make_thread_local,
                                   bool make_block_local);

        void compile_to_executable(IRNode *ir,
                                   const CompileConfig &config,
                                   Kernel *kernel,
                                   bool ad_use_stack,
                                   bool verbose,
                                   bool lower_global_access = true,
                                   bool make_thread_local = false,
                                   bool make_block_local = false,
                                   bool start_from_ast = true);

    }
}

#endif //QUINT_TRANSFORMS_H

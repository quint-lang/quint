//
// Created by BY210033 on 2023/4/6.
//
#include "quint/ir/ir.h"
#include "quint/ir/transforms.h"
#include "quint/analysis/analysis.h"
#include "quint/program/compile_config.h"
#include "quint/program/kernel.h"
#include "quint/system/profiler.h"
#include "quint/ir/pass.h"

namespace quint::lang {

    namespace irpass {

        std::function<void(const std::string &)>
        make_pass_printer(bool verbose, const std::string &kernel_name, IRNode *ir) {
            if (!verbose) {
                return [](const std::string &) {};
            }
            return [ir, kernel_name](const std::string &pass) {
                QUINT_INFO("[{}] {}:", kernel_name, pass);
                std::cout << std::flush;
                irpass::re_id(ir);
                irpass::print(ir);
                std::cout << std::flush;
            };
        }

        void
        compile_to_offloads(IRNode *ir, const CompileConfig &config, Kernel *kernel, bool verbose, bool ad_use_stack,
                            bool start_from_ast) {
            QUINT_AUTO_PROF;

            auto print = make_pass_printer(verbose, kernel->get_name(), ir);
            print("Initial IR");

            if (start_from_ast) {
                irpass::fronted_type_check(ir);
                irpass::lower_ast(ir);
                print("Lowered");
            }

            irpass::eliminate_immutable_local_vars(ir);
            print("Immutable local vars eliminated");

            if (config.real_matrix_scalarize) {
                irpass::scalarize(ir, config);

                // Remove redundant MatrixInitStmt inserted during sclarization
                irpass::die(ir);
                print("Scalarized");
            }

            // todo lower matrix

            irpass::type_check(ir, config);
            print("Type checked");
            irpass::analysis::verify(ir);

            if (kernel->is_evaluator) {
                irpass::demote_operations(ir, config);
                print("Operations demoted");
                irpass::offload(ir, config);
                print("Offloaded");
                irpass::analysis::verify(ir);
                return;
            }

            // todo handler cuda
            irpass::bit_loop_vectorize(ir);
            irpass::type_check(ir, config);
            print("Bit loop vectorized");
            irpass::analysis::verify(ir);

            irpass::full_simplify(ir, config, {false, false, kernel->program});
            print("Simplified I");
            irpass::analysis::verify(ir);

            if (config.check_out_of_bound) {
                irpass::check_out_of_bound(ir, config, {kernel->get_name()});
                print("Bound checked");
                irpass::analysis::verify(ir);
            }

            irpass::flag_access(ir);
            print("Access flagged I");
            irpass::analysis::verify(ir);

            irpass::full_simplify(ir, config, {false, false, kernel->program});
            print("Simplified II");
            irpass::analysis::verify(ir);

            irpass::offload(ir, config);
            print("Offloaded");
            irpass::analysis::verify(ir);

            if (config.opt_level > 0 && config.cfg_optimization) {
                irpass::cfg_optimization(ir, false, false, !config.real_matrix_scalarize);
                print("CFG optimized");
                irpass::analysis::verify(ir);
            }

            irpass::flag_access(ir);
            print("Access flagged II");

            irpass::full_simplify(ir, config, {false, false, kernel->program});
            print("Simplified III");
            irpass::analysis::verify(ir);
        }

        void offload_to_executable(IRNode *ir,
                                   const CompileConfig &config,
                                   Kernel *kernel,
                                   bool verbose,
                                   bool determine_ad_stack_size,
                                   bool lower_global_access,
                                   bool make_thread_local,
                                   bool make_block_local) {
            QUINT_AUTO_PROF

            auto print = make_pass_printer(verbose, kernel->get_name(), ir);

            auto amgr = std::make_unique<AnalysisManager>();

            print("Start offload_to_executable");
            irpass::analysis::verify(ir);

            if (config.detect_read_only) {
                irpass::detect_read_only(ir);
                print("Detect read-only accesses");
            }

            irpass::demote_atomics(ir, config);
            print("Atomics demoted I");
            irpass::analysis::verify(ir);
            if (config.cache_loop_invariant_global_vars) {
                irpass::cache_loop_invariant_global_vars(ir, config);
                print("Cache loop-invariant global vars");
            }

            if (config.demote_dense_struct_for) {
                irpass::demote_dense_struct_fors(ir, config.packed);
                irpass::type_check(ir, config);
                print("Dense struct-for demoted");
                irpass::analysis::verify(ir);
            }

            if (make_thread_local) {
                irpass::make_thread_local(ir, config);
                print("Thread-localized");
            }

            if (make_block_local) {
                irpass::make_block_local(ir, config, {kernel->get_name()});
                print("Block-localized");
            }

            irpass::demote_atomics(ir, config);
            print("Atomics demoted II");
            irpass::analysis::verify(ir);

            irpass::remove_range_assumption(ir);
            print("Range assumption removed");

            irpass::remove_loop_unique(ir);
            print("Loop unique removed");
            irpass::analysis::verify(ir);

            if (lower_global_access) {
                irpass::full_simplify(ir, config, {false, false, kernel->program});
                print("Simplified before lower access");
                irpass::lower_access(ir, config, {kernel->no_activate, true});
                print("Access lowered");
                irpass::analysis::verify(ir);

                irpass::die(ir);
                print("DIE");
                irpass::analysis::verify(ir);

                irpass::flag_access(ir);
                print("Access flagged III");
                irpass::analysis::verify(ir);
            }

            irpass::demote_operations(ir, config);
            print("Operations demoted");

            irpass::full_simplify(ir, config, {lower_global_access, false, kernel->program});
            print("Simplified IV");

            if (determine_ad_stack_size) {
                irpass::determine_ad_stack_size(ir, config);
                print("AD stack size determined");
            }

            irpass::type_check(ir, config);
            irpass::analysis::verify(ir);
        }

        void
        compile_to_executable(IRNode *ir, const CompileConfig &config, Kernel *kernel, bool ad_use_stack, bool verbose,
                              bool lower_global_access, bool make_thread_local, bool make_block_local,
                              bool start_from_ast) {
            QUINT_AUTO_PROF

            compile_to_offloads(ir, config, kernel, verbose, ad_use_stack, start_from_ast);
            offload_to_executable(ir, config, kernel, verbose, ad_use_stack, lower_global_access,
                                  make_block_local, make_block_local);
        }

    }

}
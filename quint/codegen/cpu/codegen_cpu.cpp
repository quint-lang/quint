//
// Created by BY210033 on 2023/3/22.
//
#include "quint/codegen/cpu/codegen_cpu.h"
#include "quint/system/profiler.h"
#include "quint/runtime/program_impls/llvm/llvm_program.h"
#include "quint/rhi/device.h"

namespace quint::lang {

    class TaskCodeGenCPU : public TaskCodegenLLVM {
    public:
        using IRVisitor::visit;

        TaskCodeGenCPU(Kernel *kernel, IRNode *ir)
        : TaskCodegenLLVM(kernel, ir, nullptr) {
            QUINT_AUTO_PROF
        }

    };

    std::unique_ptr<TaskCodegenLLVM> KernelCodeGenCPU::make_codegen_llvm(Kernel *kernel, IRNode *ir) {
        return std::make_unique<TaskCodeGenCPU>(kernel, ir);
    }

    LLVMCompiledTask KernelCodeGenCPU::compile_task(std::unique_ptr<llvm::Module> &&module, OffloadedStmt *stmt) {
        TaskCodeGenCPU gen(kernel, stmt);
        return gen.run_compilation();
    }

    FunctionType KernelCodeGenCPU::compile_to_function() {
        QUINT_AUTO_PROF
        auto *llvm_prog = get_llvm_program(prog);
        auto *ctx = llvm_prog->get_llvm_context();

        CPUModuleToFunctionConverter converter(
                ctx, get_llvm_program(prog)->get_runtime_executor());
        return converter.convert(kernel, compile_kernel_to_module());
    }

    FunctionType
    CPUModuleToFunctionConverter::convert(const std::string &kernel_name, const std::vector<LLVMLaunchArgInfo> &args,
                                          LLVMCompiledKernel data) const {
        QUINT_AUTO_PROF
        auto jit_module = qlctx->create_jit_module(std::move(data.module));
        using TaskFunc = int32 (*)(void *);
        std::vector<TaskFunc > task_funcs;
        task_funcs.reserve(data.tasks.size());
        for (auto &task : data.tasks) {
            auto *func_ptr = jit_module->lookup_function(task.name);
            QUINT_ASSERT_INFO(func_ptr, "Offloaded datum function {} not found",
                              task.name)
            task_funcs.push_back((TaskFunc)(func_ptr));
        }

        return [executor = this->executor_, args, kernel_name,
                task_funcs](RuntimeContext &context) {
            QUINT_TRACE("Launching kernel {}", kernel_name);
            for (int i = 0; i < (int) args.size(); ++i) {
                if (args[i].is_array &&
                    context.device_allocation_type[i] !=
                        RuntimeContext::DevAllocType::kNone &&
                    context.array_runtime_sizes[i] > 0) {
                    DeviceAllocation *ptr =
                            static_cast<DeviceAllocation *>(context.get_arg<void *>(i));
                    uint64 host_ptr = (uint64)executor->get_ndarray_alloc_info_ptr(*ptr);
                    context.set_arg(i, host_ptr);
                    context.set_array_device_allocation_type(i, RuntimeContext::DevAllocType::kNone);
                }
            }
            for (auto task : task_funcs) {
                task(&context);
            }
        };
    }

}
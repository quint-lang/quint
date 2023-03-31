//
// Created by BY210033 on 2023/3/30.
//

#ifndef QUINT_JIT_SESSION_H
#define QUINT_JIT_SESSION_H

#include "runtime/llvm/llvm_fwd.h"
#include "util/lang_util.h"
#include "jit/jit_module.h"

namespace quint::lang {

    class QuintLLVMContext;
    struct CompileConfig;

    class JITSession {
    protected:
        QuintLLVMContext *ctx_;
        CompileConfig *config_;

        std::vector<std::unique_ptr<JITModule>> modules;

    public:
        JITSession(QuintLLVMContext *ctx, CompileConfig *config);

        virtual JITModule *add_module(std::unique_ptr<llvm::Module> M, int max_reg = 0) = 0;

        virtual void *lookup(const std::string name) {
            QUINT_NOT_IMPLEMENTED
        }

        virtual llvm::DataLayout get_data_layout() = 0;

        static std::unique_ptr<JITSession> create(QuintLLVMContext *ctx,
                                                  CompileConfig *config);

        virtual void global_optimize_module(llvm::Module *module) {
        }

        virtual ~JITSession() = default;
    };

}

#endif //QUINT_JIT_SESSION_H

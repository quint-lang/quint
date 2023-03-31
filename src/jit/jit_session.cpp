//
// Created by BY210033 on 2023/3/10.
//
#include "jit/jit_session.h"

namespace quint::lang {

#ifdef QUINT_WITH_LLVM
    std::unique_ptr<JITSession> create_llvm_jit_session_cpu(QuintLLVMContext *ctx, CompileConfig *config);
#endif

    JITSession::JITSession(QuintLLVMContext *ctx, CompileConfig *config)
        : ctx_(ctx), config_(config) {
    }

    std::unique_ptr<JITSession> JITSession::create(QuintLLVMContext *ctx, CompileConfig *config) {
#ifdef QUINT_WITH_LLVM
        return create_llvm_jit_session_cpu(ctx, config);
#else
        QUINT_NOT_IMPLEMENTED
#endif
    }

}

//
// Created by BY210033 on 2023/3/15.
//

#ifndef QUINT_PROGRAM_IMPL_H
#define QUINT_PROGRAM_IMPL_H

#include "program/compile_config.h"
#include "program/kernel_profiler.h"

namespace quint::lang {

    struct RuntimeContext;

    class ProgramImpl {
        CompileConfig *config;

    public:
        explicit ProgramImpl(CompileConfig &config);

        virtual void prepare_runtime_context(RuntimeContext *ctx) {
        }
    };
}

#endif //QUINT_PROGRAM_IMPL_H

//
// Created by BY210033 on 2023/4/11.
//

#ifndef QUINT_CONSTANT_FOLD_H
#define QUINT_CONSTANT_FOLD_H

#include "quint/ir/pass.h"

namespace quint::lang {

    class ConstantFoldPass : public Pass {
    public:
        static const PassID id;

        struct Args {
            Program *program;
        };
    };

}

#endif //QUINT_CONSTANT_FOLD_H

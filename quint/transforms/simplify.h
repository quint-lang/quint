//
// Created by BY210033 on 2023/4/11.
//

#ifndef QUINT_SIMPLIFY_H
#define QUINT_SIMPLIFY_H

#include "quint/ir/pass.h"

namespace quint::lang {
    class FullSimplifyPass : public Pass {
    public:
        static const PassID id;

        struct Args {
            bool after_lower_access;
            bool autodiff_enabled;
            Program *program;
        };
    };
}

#endif //QUINT_SIMPLIFY_H

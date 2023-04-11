//
// Created by BY210033 on 2023/4/11.
//

#ifndef QUINT_CHECK_OUT_OF_BOUND_H
#define QUINT_CHECK_OUT_OF_BOUND_H

#include "quint/ir/pass.h"

namespace quint::lang {

    class CheckOutOfBoundPass : public Pass {
    public:
        static const PassID id;

        struct Args {
            std::string kernel_name;
        };
    };

}

#endif //QUINT_CHECK_OUT_OF_BOUND_H

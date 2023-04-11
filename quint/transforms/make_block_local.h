//
// Created by BY210033 on 2023/4/11.
//

#ifndef QUINT_MAKE_BLOCK_LOCAL_H
#define QUINT_MAKE_BLOCK_LOCAL_H

#include "quint/ir/pass.h"

namespace quint::lang {

    class MakeBlockLocalPass : public Pass {
    public:
        static const PassID id;

        struct Args {
            std::string kernel_name;
        };
    };

}

#endif //QUINT_MAKE_BLOCK_LOCAL_H

//
// Created by BY210033 on 2023/4/11.
//

#ifndef QUINT_LOWER_ACCESS_H
#define QUINT_LOWER_ACCESS_H

#include "quint/ir/pass.h"

namespace quint::lang {

        class LowerAccessPass : public Pass {
        public:
            static const PassID id;

            struct Args {
                std::vector<SNode *> kernel_forces_no_activate;
                bool lower_atomic;
            };
        };
}

#endif //QUINT_LOWER_ACCESS_H

//
// Created by BY210033 on 2023/4/17.
//

#ifndef QUINT_FRONT_LOGGER_H
#define QUINT_FRONT_LOGGER_H

#include "llvm/Support/Path.h"
#include "quint/common/logger.h"

namespace quint::frontend {
    enum MessageGroupPos {
        NONE = 0,
        HEAD,
        MID,
        LAST,
    };

    void compilationError(const std::string &msg, const std::string &file = "",
                          int line = 0, int col = 0, int len = 0, int errorCode = -1,
                            bool terminate = true, MessageGroupPos pos = NONE);

    void compilationWarning(const std::string &msg, const std::string &file = "",
                            int line = 0, int col = 0, int len = 0, int errorCode = -1,
                            bool terminate = false, MessageGroupPos pos = NONE);
}

#endif //QUINT_FRONT_LOGGER_H

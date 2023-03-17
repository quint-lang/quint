//
// Created by BY210033 on 2023/2/7.
//

#ifndef QUINT_ERR_HANDLER_H
#define QUINT_ERR_HANDLER_H

#include "common/typedefs.h"

namespace quint {
    /* throw definitions */

    /* Check the nan */
    inline void check_nan(const complex_t& m)
    {
#ifndef QRAM_Release
        if (std::isnan(m.real()) || std::isnan(m.imag()))
        {
            throw std::runtime_error("Nan!");
        }
#endif
    }

    /* Check the nan */
    inline void check_nan(const double& m)
    {
#ifndef QRAM_Release
        if (std::isnan(m))
        {
            throw std::runtime_error("Nan!");
        }
#endif
    }

    inline void throw_not_implemented()
    {
#ifndef QRAM_Release
        throw std::runtime_error("Not implemented.");
#endif
    }

    inline void throw_bad_switch_case()
    {
#ifndef QRAM_Release
        throw std::runtime_error("Impossible switch branch.");
#endif
    }

    inline void throw_invalid_input()
    {
#ifndef QRAM_Release
        throw std::runtime_error("Invalid input.");
#endif
    }

    inline void throw_bad_result()
    {
#ifndef QRAM_Release
        throw std::runtime_error("Bad result.");
#endif
    }

    inline void throw_general_runtime_error()
    {
#ifndef QRAM_Release
        throw std::runtime_error("Runtime error.");
#endif
    }
}

#endif //QUINT_ERR_HANDLER_H

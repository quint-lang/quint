//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_IO_H
#define QUINT_IO_H

namespace quint {
    template <typename First, typename... Path>
    inline std::string join_path(First &&path, Path &&...others) {
        if constexpr (sizeof...(others) == 0) {
            return std::string(path);
        } else {
            return std::string(path) + "/" +
                   join_path(std::forward<Path>(others)...);
        }
        return "";
    }
}

#endif //QUINT_IO_H

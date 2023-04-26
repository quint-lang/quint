//
// Created by BY210033 on 2023/4/17.
//

#ifndef QUINT_PARSE_COMMON_H
#define QUINT_PARSE_COMMON_H

#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#define CAST(s, T) dynamic_cast<T *>(s.get())

namespace quint::ast {

    std::string getAbsolutePath(const std::string &path);

}

#endif //QUINT_PARSE_COMMON_H

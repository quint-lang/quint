//
// Created by BY210033 on 2023/3/31.
//

#ifndef QUINT_OFFLINE_CACHE_UTIL_H
#define QUINT_OFFLINE_CACHE_UTIL_H

#include <string>

#include "common/core.h"

namespace quint::lang {

    struct CompileConfig;
    class Program;
    class IRNode;
    class SNode;
    class Kernel;

    std::string get_hashed_offline_cache_key_of_snode(SNode *snode);
    std::string get_hashed_offline_cache_key(const CompileConfig *config, Kernel *kernel);

    void gen_offline_cache_key(Program *prog, IRNode *ast, std::ostream *os);

}

#endif //QUINT_OFFLINE_CACHE_UTIL_H

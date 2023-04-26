//
// Created by BY210033 on 2023/4/17.
//

#ifndef QUINT_DSL_H
#define QUINT_DSL_H

#include <functional>
#include <string>
#include <vector>

#include "llvm/Passes/PassBuilder.h"

namespace quint {
    class DSL {
    public:
        struct Info {
            std::string name;
            std::string description;
            std::string version;
            std::string url;
            std::string supported;
            std::string stdlibPath;
            std::string dylibPath;
        };

//        using KeywordCallback =
//                std::function<ast

        virtual ~DSL() noexcept = default;

        virtual void addLLVMPasses(llvm::PassBuilder *pb, bool debug) {}
    };
}

#endif //QUINT_DSL_H

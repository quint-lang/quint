//
// Created by BY210033 on 2023/3/1.
//

#ifndef QUINT_GEN_MODULE_H
#define QUINT_GEN_MODULE_H

#include <llvm/ADT/StringMap.h>
#include <llvm/Support/raw_ostream.h>

namespace quint {

    class TypeMap {
        llvm::StringMap<llvm::StringRef> cppMap;
        llvm::StringMap<llvm::StringRef> argumentsMap;
        llvm::StringMap<llvm::StringRef> resultMap;

    public:
        TypeMap() {
#define CPPMAP(key, value) cppMap.insert(std::pair(key, value));
#define RESULTSMAP(key, value) resultMap.insert(std::pair(key, value));
#define ARGUMENTSMAP(key, value) argumentsMap.insert(std::pair(key, value));
#include "TypeMap.def"
        }

        llvm::StringRef findCppMap(llvm::StringRef value);
        llvm::StringRef findArgumentMap(llvm::StringRef value);
        llvm::StringRef findResultMap(llvm::StringRef value);
    };

    class CGModule {
        llvm::raw_fd_ostream &os;
        TypeMap typeMap;

    public:
        CGModule(llvm::raw_fd_ostream &os): os(os) {}

    };

}

#endif //QUINT_GEN_MODULE_H

//
// Created by BY210033 on 2023/4/17.
//
#include "dsl/plugins.h"

namespace quint {

    llvm::Expected<Plugin *> PluginManager::load(const std::string &path) {
        return plugins.back().get();
    }

}
//
// Created by BY210033 on 2023/4/17.
//

#ifndef QUINT_PLUGINS_H
#define QUINT_PLUGINS_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "compiler/error.h"
#include "dsl/dsl.h"
#include "llvm/Support/DynamicLibrary.h"

namespace quint {

    struct Plugin {
        std::unique_ptr<DSL> dsl;
        DSL::Info info;
        llvm::sys::DynamicLibrary handle;

        Plugin(std::unique_ptr<DSL> dsl, DSL::Info info, llvm::sys::DynamicLibrary handle)
            : dsl(std::move(dsl)), info(std::move(info)), handle(std::move(handle)) {}
    };

    class PluginManager {
    private:
        std::string argv0;
        std::vector<std::unique_ptr<Plugin>> plugins;

    public:
        PluginManager(const std::string &argv0) : argv0(argv0), plugins() {}

        llvm::Expected<Plugin *> load(const std::string &path);
    };

}

#endif //QUINT_PLUGINS_H

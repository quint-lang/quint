//
// Created by BY210033 on 2023/4/13.
//

#ifndef QUINT_COMPILER_H
#define QUINT_COMPILER_H

#include <string>
#include <vector>

#include "compiler/error.h"
#include "dsl/plugins.h"

namespace quint {

    class Compiler {
    public:
        enum Mode {
            DEBUG,
            RELEASE,
            JIT,
        };

    private:
        std::string argv0;
        bool debug;
        std::string input;
        std::unique_ptr<PluginManager> plm;

        llvm::Error parse(bool isCode, const std::string &file, const std::string &code,
                          int startLine, int testFlags,
                          const std::unordered_map<std::string, std::string> &defines);

    public:
        Compiler(const std::string &argv0, Mode mode,
                 const std::vector<std::string> &disabledPasses = {}, bool isTest = false);

        explicit Compiler(const std::string &argv0, bool debug = false,
                          const std::vector<std::string> &disabledPasses = {},
                          bool isTest = false)
            : Compiler(argv0, debug ? Mode::DEBUG : Mode::RELEASE, disabledPasses, isTest) {}

        std::string getInput() const { return input; }

        llvm::Error loadPlugin(const std::string &plugin);

        llvm::Error
        parseFile(const std::string &file, int testFlags = 0,
                  const std::unordered_map<std::string, std::string> &defines = {});

        llvm::Error compile();
    };

}

#endif //QUINT_COMPILER_H

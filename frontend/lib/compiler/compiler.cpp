//
// Created by BY210033 on 2023/4/13.
//
#include "compiler/compiler.h"
#include "parser/parser_common.h"

namespace quint {

    Compiler::Compiler(const std::string &argv0, Compiler::Mode mode, const std::vector<std::string> &disabledPasses,
                       bool isTest)
        : argv0(argv0), debug(mode == Mode::DEBUG), input(),
          plm(std::make_unique<PluginManager>(argv0)) {

    }

    llvm::Error Compiler::loadPlugin(const std::string &plugin) {
        auto result = plm->load(plugin);
        if (auto err = result.takeError())
            return err;

        auto *p = *result;
        if (!p->info.stdlibPath.empty()) {
            // cache->pluginImportPaths.push_back(p->info.stdlibPath);
        }

        // todo handler ExprKeywords & BlockKeywords
//        for (auto &kw : p->dsl.) {
//
//        }
        // todo handler AddIRPasses
//        p->dsl->add();
        return llvm::Error::success();
    }

    llvm::Error
    Compiler::parse(bool isCode, const std::string &file, const std::string &code, int startLine, int testFlags,
                    const std::unordered_map<std::string, std::string> &defines) {
        input = file;
        std::string abspath = (file != "-") ? ast::getAbsolutePath(file) : file;
        try {

        } catch (const std::exception &e) {
            return llvm::make_error<llvm::StringError>(e.what(), llvm::inconvertibleErrorCode());
        }
        return llvm::Error::success();
    }

    llvm::Error Compiler::parseFile(const std::string &file, int testFlags,
                                    const std::unordered_map<std::string, std::string> &defines) {
        return parse(false, file, "", 0, testFlags, defines);
    }

    llvm::Error Compiler::compile() {
        return llvm::Error::success();
    }

}
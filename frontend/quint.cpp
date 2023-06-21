//
// Created by BY210033 on 2023/2/13.
//
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "compiler/compiler.h"
#include "quint/common/logger.h"
#include "quint/config/config.h"
#include "compiler/error.h"
#include "util/front_logger.h"
#include "parser/parser.h"

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_os_ostream.h>
namespace cl = llvm::cl;
using namespace quint;

static cl::opt<std::string> inputFilename(cl::Positional, cl::desc("<input qrunes file>"),
                                          cl::init("-"), cl::value_desc("filename"));

void versMsg(llvm::raw_ostream &out) {
    out << QUINT_VERSION_MAJOR << "." << QUINT_VERSION_MINOR << "." << QUINT_VERSION_PATCH
        << "\n";
}

void showCommandsAndExit() {
    QUINT_ERROR("Available commands: quint <run|build|jit>")
}

void display(const quint::error::ParseErrorInfo &err) {
    using quint::frontend::MessageGroupPos;
    for (auto &group : err) {
        for (auto &msg : group) {
            MessageGroupPos pos = MessageGroupPos::NONE;
            if (&msg == &group.front()) {
                pos = MessageGroupPos::HEAD;
            } else if (&msg == &group.back()) {
                pos = MessageGroupPos::LAST;
            } else {
                pos = MessageGroupPos::MID;
            }
            quint::frontend::compilationError(msg.getMsg(), msg.getFile(), msg.getLine(),
                                              msg.getColumn(), msg.getLength(), msg.getErrorCode(),
                                              false, pos);
        }

    }
};

enum OptMode { Debug, Release };

std::unique_ptr<quint::Compiler> processSource(const std::vector<const char *> &args,
                                               bool standalone) {
    cl::opt<std::string> input(cl::Positional, cl::desc("<input file>"),
                               llvm::cl::init("-"), cl::value_desc("filename"));
    auto regs = cl::getRegisteredOptions();
    cl::opt<OptMode> optMode(
            cl::desc("optimization mode"),
            cl::values(
                    clEnumValN(Debug, regs.find("debug") != regs.end() ? "default" : "debug",
                               "Turn off compiler optimizations and shows backtraces"),
                    clEnumValN(Release, "release", "Release mode")),
            cl::init(Release));
    cl::list<std::string> defines(
            "D", cl::Prefix,
            llvm::cl::desc("Define a macro. Use -DNAME=VALUE to define a macro with a value"));
    cl::list<std::string> disabledOpts(
            "disable-opt", cl::desc("Disable a specific optimization pass"));
    cl::list<std::string> plugins("plugin",
                                  cl::desc("Load a plugin. Use -plugin=NAME to load a plugin"));
    cl::opt<std::string> log("log", cl::desc("Enable given log streams"));
    cl::ParseCommandLineOptions(args.size(), args.data());

    std::unordered_map<std::string, std::string> definesMap;
    for (const auto &def : defines) {
        auto pos = def.find('=');
        if (pos == std::string::npos || !pos) {
            QUINT_WARN("ignoring malformed define: {}", def);
            continue;
        }

        auto name = def.substr(0, pos);
        auto value = def.substr(pos + 1);

        if (definesMap.find(name) != definesMap.end()) {
            QUINT_WARN("ignoring duplicate define: {}", name);
            continue;
        }

        definesMap.emplace(name, value);
    }

    const bool isDebug = (optMode == OptMode::Debug);
    std::vector<std::string> disabledPasses(disabledOpts);
    auto compiler = std::make_unique<quint::Compiler>(args[0], isDebug, disabledPasses, false);
    // todo set MLIRVisitor

    for (const auto &plugin : plugins) {
        bool failed = false;
        llvm::handleAllErrors(
                compiler->loadPlugin(plugin),
                [&failed](const quint::error::PluginErrorInfo &e) {
                    quint::frontend::compilationError(e.getMessage(), "",
                                                      0, 0, 0, -1, false);
                    failed = true;
                });
        if (failed)
            return {};
    }

    bool failed = false;
    int testFlags = 0;
    if (auto *tf = getenv("QUINT_TEST_FLAGS"))
        testFlags = std::stoi(tf);
    llvm::handleAllErrors(
            compiler->parseFile(input, testFlags, definesMap),
            [&failed](const quint::error::ParseErrorInfo &e) {
                display(e);
                failed = true;
            });
    if (failed)
        return {};

    {
        llvm::cantFail(compiler->compile());
    }
    return compiler;
}

int runMode(std::vector<const char *> &args) {
    cl::list<std::string> libs(
            "l", cl::desc("Load a link the specified library"));
    cl::list<std::string> progArgs(cl::ConsumeAfter, cl::desc("<program arguments>..."));
    auto compiler = processSource(args, false);
    if (!compiler)
        return EXIT_FAILURE;
    std::vector<std::string> libPaths;
    std::vector<std::string> argsVec;
    argsVec.insert(argsVec.begin(), compiler->getInput());
    // todo run JIT
    return EXIT_SUCCESS;
}

int buildMode(std::vector<const char *> &args, const std::string &argv0) {
    return 1;
}

int jitMode(std::vector<const char *> &args) {
    return 1;
}

int otherMode(std::vector<const char *> &args) {
    cl::opt<std::string> input(cl::Positional, cl::desc("<mode>"));
    llvm::cl::extrahelp("\nMODES:\n\n"
                        "run <file> <args>  Run a quint program interactively\n"
                        "build <file>       Build a quint program to binary file\n"
                        "jit <file>         Run a quint program in JIT mode\n");
    llvm::cl::ParseCommandLineOptions(args.size(), args.data());
    if (!input.empty())
        showCommandsAndExit();
    return EXIT_SUCCESS;
}

//int main(int argc, const char **argv) {
//    if (argc < 2)
//        showCommandsAndExit();
//
//    cl::SetVersionPrinter(versMsg);
//    std::vector<const char *> args{argv[0]};
//    for (int i = 1; i < argc; i++) {
//        args.push_back(argv[i]);
//    }
//
//    std::string mode(argv[1]);
//    std::string argv0 = std::string(args[0]) + " " + mode;
//    if (mode == "run") {
//        args[0] = argv0.data();
//        return runMode(args);
//    } else if (mode == "build") {
//        const char *oldArgv0 = args[0];
//        args[0] = argv0.data();
//        return buildMode(args, oldArgv0);
//    } else if (mode == "jit") {
//        args[0] = argv0.data();
//        return jitMode(args);
//    } else {
//        return otherMode(args);
//    }
//}

std::unique_ptr<ast::ModuleScope> parseInputFile(llvm::StringRef filename) {
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> fileOrErr =
        llvm::MemoryBuffer::getFileOrSTDIN(filename);
    if (std::error_code ec = fileOrErr.getError())
    {
        llvm::errs() << "Could not open input file: " << ec.message() <<"\n";
        return nullptr;
    }
    auto buffer = fileOrErr.get()->getBuffer();
    LexerBuffer lexer(buffer.begin(), buffer.end(), std::string(filename));
    Parser parser(lexer);
    return parser.parse();
};

int run()
{
    auto moduleScope = parseInputFile(inputFilename);
    return 0;
}

int main(int argc, char** argv)
{
    cl::ParseCommandLineOptions(argc, argv, "qrunes compiler\n");
    return run();
}

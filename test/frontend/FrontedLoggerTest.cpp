//
// Created by BY210033 on 2023/4/17.
//
#include "spdlog/fmt/fmt.h"
#include <string>

enum MessageGroupPos {
    NONE = 0,
    HEAD,
    MID,
    LAST,
};

void compilationMessage(const std::string &header, const std::string &msg,
                        const std::string &file, int line, int col, int len,
                        int errorCode, MessageGroupPos pos) {
    switch (pos) {
        case MessageGroupPos::NONE:
            break;
        case MessageGroupPos::HEAD:
            break;
        case MessageGroupPos::MID:
            fmt::print("├─ ");
            break;
        case MessageGroupPos::LAST:
            fmt::print("╰─ ");
            break;
    }

    fmt::print("\033[1m");
    if (!file.empty()) {
        auto f = file.substr(file.rfind('/') + 1);
        fmt::print("{}", f == "-" ? "<stdin>" : f);
    }
    if (line > 0)
        fmt::print(":{}", line);
    if (col > 0)
        fmt::print(":{}", col);
    if (len > 0)
        fmt::print("-{}", col + len);
    if (!file.empty())
        fmt::print(": ");
    fmt::print("{}\033[1m {}\033[0m{}\n", header, msg,
               errorCode != -1
               ? fmt::format(" (see https://exaloop.io/error/{:04d})", errorCode)
               : "");
}

void compilationError(const std::string &msg, const std::string &file, int line, int col, int len, int errorCode,
                      bool terminate, MessageGroupPos pos=MessageGroupPos::NONE) {
    compilationMessage("\033[1;31merror:\033[0m", msg, file, line, col, len, errorCode,
                       pos);

    if (terminate)
        exit(EXIT_FAILURE);
}

void compilationWarning(const std::string &msg, const std::string &file, int line, int col, int len, int errorCode,
                        bool terminate, MessageGroupPos pos=MessageGroupPos::NONE) {
    compilationMessage("\033[1;33mwarning:\033[0m", msg, file, line, col, len, errorCode,
                       pos);

    if (terminate)
        exit(EXIT_FAILURE);
}

int main() {
    compilationError("can't load plugin qrunes", "", 0, 0, 0, -1, false);
}
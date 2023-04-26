//
// Created by BY210033 on 2023/4/17.
//

#ifndef QUINT_AST_ERROR_H
#define QUINT_AST_ERROR_H

namespace quint {
    struct SrcInfo {
        std::string file;
        int line = 0;
        int col = 0;
        int len = 0;
        int id; /// used to differentiate different instances

        SrcInfo(std::string file, int line, int col, int len)
                : file(std::move(file)), line(line), col(col), len(len), id(0) {
            static int nextId = 0;
            id = nextId++;
        };

        SrcInfo() : SrcInfo("", 0, 0, 0) {}

        bool operator==(const SrcInfo &src) const { return id == src.id; }
    };
} // namespace quint

namespace quint::exc {
    class ParserException : public std::runtime_error {
    public:
        /// These vectors (stacks) store an error stack-trace.
        std::vector<SrcInfo> locations;
        std::vector<std::string> messages;
        int errorCode = -1;

    public:
        ParserException(int errorCode, const std::string &msg, const SrcInfo &info) noexcept
                : std::runtime_error(msg), errorCode(errorCode) {
            messages.push_back(msg);
            locations.push_back(info);
        }
        ParserException() noexcept : std::runtime_error("") {}
        ParserException(int errorCode, const std::string &msg) noexcept
                : ParserException(errorCode, msg, {}) {}
        explicit ParserException(const std::string &msg) noexcept
                : ParserException(-1, msg, {}) {}
        ParserException(const ParserException &e) noexcept
                : std::runtime_error(e), locations(e.locations), messages(e.messages),
                  errorCode(e.errorCode){};

        /// Add an error message to the current stack trace
        void trackRealize(const std::string &msg, const SrcInfo &info) {
            locations.push_back(info);
            messages.push_back("during the realization of " + msg);
        }

        /// Add an error message to the current stack trace
        void track(const std::string &msg, const SrcInfo &info) {
            locations.push_back(info);
            messages.push_back(msg);
        }
    };
}

#endif //QUINT_AST_ERROR_H

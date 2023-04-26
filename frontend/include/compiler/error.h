//
// Created by BY210033 on 2023/4/17.
//

#ifndef QUINT_ERROR_H
#define QUINT_ERROR_H

#include "llvm/Support/Error.h"
#include "ast/error.h"

#include <string>
#include <vector>

namespace quint::error {

    class Message {
        std::string msg;
        std::string file;
        int line = 0;
        int col = 0;
        int len = 0;
        int errorCode = -1;

    public:
        explicit Message(const std::string &msg, const std::string &file = "", int line = 0,
                         int col = 0, int len = 0, int errorCode = -1)
                : msg(msg), file(file), line(line), col(col), len(len), errorCode(errorCode) {}

        std::string getMsg() const { return msg; }
        std::string getFile() const { return file; }
        int getLine() const { return line; }
        int getColumn() const { return col; }
        int getLength() const { return len; }
        int getErrorCode() const { return errorCode; }

        void log(llvm::raw_ostream &out) const {
            if (!getFile().empty()) {
                out << getFile();
                if (getLine() != 0) {
                    out << ":" << getLine();
                    if (getColumn() != 0) {
                        out << ":" << getColumn();
                    }
                }
                out << ": ";
            }
            out << getMsg();
        }
    };

    class ParseErrorInfo : public llvm::ErrorInfo<ParseErrorInfo> {
    private:
        std::vector<std::vector<Message>> messages;

    public:
        explicit ParseErrorInfo(const std::vector<Message> &m) : messages() {
            for (auto &msg : m) {
                messages.push_back({msg});
            }
        }
        explicit ParseErrorInfo(const exc::ParserException &e) : messages() {
            std::vector<Message> group;
            for (unsigned i = 0; i < e.messages.size(); ++i) {
                if (!e.messages[i].empty()) {
                    group.emplace_back(e.messages[i], e.locations[i].file, e.locations[i].line,
                                       e.locations[i].col, e.locations[i].len);
                }
            }
            messages.push_back(group);
        }

        auto begin() { return messages.begin(); }
        auto end() { return messages.end(); }
        auto begin() const { return messages.begin(); }
        auto end() const { return messages.end(); }

        void log(llvm::raw_ostream &out) const override {
            for (auto &group: messages) {
                for (auto &msg: group) {
                    msg.log(out);
                    out << "\n";
                }
            }
        }

        std::error_code convertToErrorCode() const override {
            return llvm::inconvertibleErrorCode();
        }

        static char ID;
    };

    class PluginErrorInfo : public llvm::ErrorInfo<PluginErrorInfo> {
    private:
        std::string message;

    public:
        explicit PluginErrorInfo(const std::string &message) : message(message) {}

        std::string getMessage() const { return message; }

        void log(llvm::raw_ostream &out) const override { out << message; }

        std::error_code convertToErrorCode() const override {
            return llvm::inconvertibleErrorCode();
        }

        static char ID;
    };

}

#endif //QUINT_ERROR_H

//
// Created by BY210033 on 2023/3/17.
//

#ifndef QUINT_EXCEPTIONS_H
#define QUINT_EXCEPTIONS_H

namespace quint::lang {

    class QuintExceptionImpl : public std::exception {
        std::string msg_;

    public:
        explicit QuintExceptionImpl(const std::string msg): msg_(msg) {
        }

        const char *what() const noexcept override {
            return msg_.c_str();
        }
    };

    class QuintTypeError : public QuintExceptionImpl {
        using QuintExceptionImpl::QuintExceptionImpl;
    };

    class QuintSyntaxError : public QuintExceptionImpl {
        using QuintExceptionImpl::QuintExceptionImpl;
    };

    class QuintIndexError : public QuintExceptionImpl {
        using QuintExceptionImpl::QuintExceptionImpl;
    };

    class QuintRuntimeError : public QuintExceptionImpl {
        using QuintExceptionImpl::QuintExceptionImpl;
    };

    class QuintAssertionError : public QuintExceptionImpl {
        using QuintExceptionImpl::QuintExceptionImpl;
    };

}

#endif //Quint_EXCEPTIONS_H

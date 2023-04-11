//
// Created by BY210033 on 2023/3/30.
//

#ifndef QUINT_JIT_MODULE_H
#define QUINT_JIT_MODULE_H

namespace quint::lang {

    class JITModule {
    public:
        JITModule() {
        }

        virtual void *lookup_function(const std::string &name) = 0;

        virtual bool direct_dispatch() const = 0;

        virtual ~JITModule() {
        }
    };

}

#endif //QUINT_JIT_MODULE_H

//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_INTERFACE_H
#define QUINT_INTERFACE_H

#include "quint/common/dict.h"

namespace quint {

    class Unit {
    public:
        Unit() {
        }

        virtual void initialize(const Config &config) {
        }

        virtual bool test() const {
            return true;
        }

        virtual std::string get_name() const {
            QUINT_NOT_IMPLEMENTED
            return "";
        }

        virtual std::string general_action(const Config &config) {
            QUINT_NOT_IMPLEMENTED
            return "";
        }

        virtual ~Unit() {
        }
    };

}

#endif //QUINT_INTERFACE_H

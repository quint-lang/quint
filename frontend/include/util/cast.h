//
// Created by BY210033 on 2023/6/13.
//

#ifndef QUINT_CAST_H
#define QUINT_CAST_H

#include "structure/base.h"

namespace quint {


    template <class T>
    bool isa(const QuintObj& value) {
        return value.classof(typeid(T));
    }

}

#endif //QUINT_CAST_H

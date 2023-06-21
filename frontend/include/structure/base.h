//
// Created by BY210033 on 2023/6/13.
//

#ifndef QUINT_BASE_H
#define QUINT_BASE_H

namespace quint {

    class QuintObj {
    public:
        virtual bool classof(const std::type_info&) const = 0;

    };

}

#endif //QUINT_BASE_H

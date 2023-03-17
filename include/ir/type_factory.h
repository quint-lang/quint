//
// Created by BY210033 on 2023/3/14.
//

#ifndef QUINT_TYPE_FACTORY_H
#define QUINT_TYPE_FACTORY_H

#include <mutex>
#include "ir/type.h"
#include "util/hash.h"

namespace quint::lang {

    class TypeFactory {
    public:
        static TypeFactory &get_instance();

        Type *get_primitive_type(PrimitiveTypeID id);

        PrimitiveType *get_primitive_int_type(int bits, bool is_signed = true);

        Type *get_pointer_type(Type *element, bool is_bit_pointer = false);

        Type *get_struct_type(const std::vector<const Type *> &elements);

    private:
        TypeFactory() {}

        std::unordered_map<PrimitiveTypeID, std::unique_ptr<Type>> primitive_types_;
        std::mutex primitive_mut_;

        std::unordered_map<std::pair<Type *, bool>,
                           std::unique_ptr<Type>,
                           hashing::Hasher<std::pair<Type *, bool>>>
            pointer_types_;
        std::mutex pointer_mut_;

        std::unordered_map<std::vector<const Type *>,
                           std::unique_ptr<Type>,
                           hashing::Hasher<std::vector<const Type *>>>
            struct_types_;
        std::mutex struct_mut_;

    };

}

#endif //QUINT_TYPE_FACTORY_H

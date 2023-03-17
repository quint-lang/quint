//
// Created by BY210033 on 2023/3/14.
//

#ifndef QUINT_TYPE_UTILS_H
#define QUINT_TYPE_UTILS_H

#include "ir/type.h"

namespace quint::lang {

    std::vector<int> data_type_shape(DataType t);

    std::string data_type_name(DataType t);

    int data_type_size(DataType t);

    inline bool is_integral(DataType dt) {
        // todo quantum integer type
        return dt->is_primitive(PrimitiveTypeID::i8) ||
                dt->is_primitive(PrimitiveTypeID::i16) ||
                dt->is_primitive(PrimitiveTypeID::i32) ||
                dt->is_primitive(PrimitiveTypeID::i64) ||
                dt->is_primitive(PrimitiveTypeID::u8) ||
                dt->is_primitive(PrimitiveTypeID::u16) ||
                dt->is_primitive(PrimitiveTypeID::u32) ||
                dt->is_primitive(PrimitiveTypeID::u64);
    }

    inline bool is_signed(DataType dt) {
        // shall we return false if is integral returns false?
        QUINT_ASSERT(is_integral(dt))
        return dt->is_primitive(PrimitiveTypeID::i8) ||
               dt->is_primitive(PrimitiveTypeID::i16) ||
               dt->is_primitive(PrimitiveTypeID::i32) ||
               dt->is_primitive(PrimitiveTypeID::i64);
    }

    inline bool is_unsigned(DataType dt) {
        QUINT_ASSERT(is_integral(dt))
        return !is_signed(dt);
    }

    inline DataType to_unsigned(DataType dt) {
        QUINT_ASSERT(is_signed(dt))
        if (dt->is_primitive(PrimitiveTypeID::i8))
            return PrimitiveType::u8;
        else if (dt->is_primitive(PrimitiveTypeID::i16))
            return PrimitiveType::u16;
        else if (dt->is_primitive(PrimitiveTypeID::i32))
            return PrimitiveType::u32;
        else if (dt->is_primitive(PrimitiveTypeID::i64))
            return PrimitiveType::u64;
        else
            return PrimitiveType::unknown;
    }

}

#endif //QUINT_TYPE_UTILS_H

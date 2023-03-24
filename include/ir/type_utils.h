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

    template <typename T>
    inline DataType get_data_type() {
        if (std::is_same<T, float32>()) {
            return PrimitiveType::f32;
        } else if (std::is_same<T, float64>()) {
            return PrimitiveType::f64;
        } else if (std::is_same<T, bool>()) {
            return PrimitiveType::u1;
        } else if (std::is_same<T, int8>()) {
            return PrimitiveType::i8;
        } else if (std::is_same<T, int16>()) {
            return PrimitiveType::i16;
        } else if (std::is_same<T, int32>()) {
            return PrimitiveType::i32;
        } else if (std::is_same<T, int64>()) {
            return PrimitiveType::i64;
        } else if (std::is_same<T, uint8>()) {
            return PrimitiveType::u8;
        } else if (std::is_same<T, uint16>()) {
            return PrimitiveType::u16;
        } else if (std::is_same<T, uint32>()) {
            return PrimitiveType::u32;
        } else if (std::is_same<T, uint64>()) {
            return PrimitiveType::u64;
        } else {
            QUINT_NOT_IMPLEMENTED;
        }
    }

    inline bool is_real(DataType dt) {
        return dt->is_primitive(PrimitiveTypeID::f16) ||
               dt->is_primitive(PrimitiveTypeID::f32) ||
               dt->is_primitive(PrimitiveTypeID::f64);
    }

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

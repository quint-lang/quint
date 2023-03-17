//
// Created by BY210033 on 2023/3/14.
//
#include "ir/type_utils.h"

namespace quint::lang {

    std::vector<int> data_type_shape(DataType t) {
        // todo handler tensor type
        return {};
    }

    std::string data_type_name(DataType t) {
        if (!t->is<PrimitiveType>())
            return t.to_string();

        if (false) {

        }
#define PER_TYPE(i) else if (t->is_primitive(PrimitiveTypeID::i)) return #i;
#include "inc/data_type.inc.h"
#undef PER_TYPE
        else
            QUINT_NOT_IMPLEMENTED
    }

    int data_type_size(DataType t) {
        t.set_is_pointer(false);
        if (t->is_primitive(PrimitiveTypeID::f16))
            return 2;
        else if (t->is_primitive(PrimitiveTypeID::gen))
            return 0;
        else if (t->is_primitive(PrimitiveTypeID::unknown))
            return -1;

        // todo handler tensor type

#define REGISTER_DATA_TYPE(i, j) \
        else if (t->is_primitive(PrimitiveTypeID::i)) return sizeof(j)

        REGISTER_DATA_TYPE(f32, float32);
        REGISTER_DATA_TYPE(f64, float64);
        REGISTER_DATA_TYPE(i8, int8);
        REGISTER_DATA_TYPE(i16, int16);
        REGISTER_DATA_TYPE(i32, int32);
        REGISTER_DATA_TYPE(i64, int64);
        REGISTER_DATA_TYPE(u8, uint8);
        REGISTER_DATA_TYPE(u16, uint16);
        REGISTER_DATA_TYPE(u32, uint32);
        REGISTER_DATA_TYPE(u64, uint64);
#undef REGISTER_DATA_TYPE
        else {
            QUINT_NOT_IMPLEMENTED
        }
    }

}
//
// Created by BY210033 on 2023/3/14.
//
#include "quint/ir/type.h"
#include "quint/ir/type_factory.h"
#include "quint/ir/type_utils.h"

namespace quint::lang {

// This part doesn't look good, but we will remove it soon anyway.
#define PER_TYPE(x)                     \
  DataType PrimitiveType::x = DataType( \
      TypeFactory::get_instance().get_primitive_type(PrimitiveTypeID::x));

#include "quint/inc/data_type.inc.h"
#undef PER_TYPE


    DataType::DataType(): ptr_(PrimitiveType::unknown.ptr_) {
    }

    std::size_t DataType::hash() const {
        if (auto primitive = ptr_->cast<PrimitiveType>()) {
            return (std::size_t)primitive->type;
        } else if (auto pointer = ptr_->cast<PointerType>()) {
            return 10007 + DataType(pointer->get_pointee_type()).hash();
        } else {
            QUINT_NOT_IMPLEMENTED
        }
    }

    bool DataType::is_pointer() const {
        return ptr_->is<PointerType>();
    }

    void DataType::set_is_pointer(bool ptr) {
        if (ptr && !ptr_->is<PointerType>())
            ptr_ = TypeFactory::get_instance().get_pointer_type(ptr_);
        if (!ptr && ptr_->is<PointerType>())
            ptr_ = ptr_->cast<PointerType>()->get_pointee_type();
    }

    DataType DataType::ptr_removed() const {
        auto t = ptr_;
        auto ptr_type = t->cast<PointerType>();
        if (ptr_type) {
            return DataType(ptr_type->get_pointee_type());
        } else {
            return *this;
        }
    }

    std::vector<int> DataType::get_shape() const {
        // todo handler tensor type
        return {};
    }

    DataType DataType::get_element_type() const {
        // todo handler tensor type
        return *this;
    }

    bool Type::is_primitive(PrimitiveTypeID type) const {
        if (auto p = cast<PrimitiveType>()) {
            return p->type == type;
        }
        return false;
    }

    std::string PrimitiveType::to_string() const {
        return data_type_name(DataType(const_cast<PrimitiveType *>(this)));
    }

    DataType PrimitiveType::get(PrimitiveTypeID type) {
        if (false) {
        }
#define PER_TYPE(x) else if (type == PrimitiveTypeID::x) return PrimitiveType::x;
#include "quint/inc/data_type.inc.h"
#undef PER_TYPE
        else {
            QUINT_NOT_IMPLEMENTED
        }
    }

    std::string PointerType::to_string() const {
        if (is_bit_pointer_) {
            // "^" for bit-level pointers
            return fmt::format("^{}", pointee_->to_string());
        } else {
            return fmt::format("*{}", pointee_->to_string());
        }
    }

    Type *StructType::get_element_type(const std::vector<int> &indices) const {
        const Type *type_now = this;
        for (auto ind : indices) {
            // todo handler tensor type
            type_now = type_now->as<StructType>()->elements_[ind];
        }
        return (Type *)type_now;
    }

    std::string StructType::to_string() const {
        std::string s = "struct(";
        for (int i = 0; i < elements_.size(); i++) {
            if (i) {
                s += ", ";
            }
            s += std::to_string(i) + ": " + elements_[i]->to_string();
        }
        s += ")";
        return s;
    }

    std::string TypedConstant::stringify() const {
        auto dt = this->dt.ptr_removed();
        if (dt->is_primitive(PrimitiveTypeID::f32)) {
            return fmt::format("{}", val_f32);
        } else if (dt->is_primitive(PrimitiveTypeID::i32)) {
            return fmt::format("{}", val_i32);
        } else if (dt->is_primitive(PrimitiveTypeID::i64)) {
            return fmt::format("{}", val_i64);
        } else if (dt->is_primitive(PrimitiveTypeID::f64)) {
            return fmt::format("{}", val_f64);
        } else if (dt->is_primitive(PrimitiveTypeID::i8)) {
            return fmt::format("{}", val_i8);
        } else if (dt->is_primitive(PrimitiveTypeID::i16)) {
            return fmt::format("{}", val_i16);
        } else if (dt->is_primitive(PrimitiveTypeID::u8)) {
            return fmt::format("{}", val_u8);
        } else if (dt->is_primitive(PrimitiveTypeID::u16)) {
            return fmt::format("{}", val_u16);
        } else if (dt->is_primitive(PrimitiveTypeID::u32)) {
            return fmt::format("{}", val_u32);
        } else if (dt->is_primitive(PrimitiveTypeID::u64)) {
            return fmt::format("{}", val_u64);
        } else {
//            TI_P(data_type_name(dt));
            QUINT_NOT_IMPLEMENTED
            return "";
        }
    }

    bool TypedConstant::equal_type_and_value(const TypedConstant &o) const {
        if (dt != o.dt)
            return false;
        if (dt->is_primitive(PrimitiveTypeID::f32)) {
            return val_f32 == o.val_f32;
        } else if (dt->is_primitive(PrimitiveTypeID::i32)) {
            return val_i32 == o.val_i32;
        } else if (dt->is_primitive(PrimitiveTypeID::i64)) {
            return val_i64 == o.val_i64;
        } else if (dt->is_primitive(PrimitiveTypeID::f64)) {
            return val_f64 == o.val_f64;
        } else if (dt->is_primitive(PrimitiveTypeID::i8)) {
            return val_i8 == o.val_i8;
        } else if (dt->is_primitive(PrimitiveTypeID::i16)) {
            return val_i16 == o.val_i16;
        } else if (dt->is_primitive(PrimitiveTypeID::u8)) {
            return val_u8 == o.val_u8;
        } else if (dt->is_primitive(PrimitiveTypeID::u16)) {
            return val_u16 == o.val_u16;
        } else if (dt->is_primitive(PrimitiveTypeID::u32)) {
            return val_u32 == o.val_u32;
        } else if (dt->is_primitive(PrimitiveTypeID::u64)) {
            return val_u64 == o.val_u64;
        } else {
            QUINT_NOT_IMPLEMENTED
            return false;
        }
    }

    int32 &TypedConstant::val_int32() {
        QUINT_ASSERT(get_data_type<int32>() == dt)
        return val_i32;
    }

    float32 &TypedConstant::val_float32() {
        QUINT_ASSERT(get_data_type<float32>() == dt)
        return val_f32;
    }

    int64 &TypedConstant::val_int64() {
        QUINT_ASSERT(get_data_type<int64>() == dt)
        return val_i64;
    }

    float64 &TypedConstant::val_float64() {
        QUINT_ASSERT(get_data_type<float64>() == dt)
        return val_f64;
    }

    int8 &TypedConstant::val_int8() {
        QUINT_ASSERT(get_data_type<int8>() == dt)
        return val_i8;
    }

    int16 &TypedConstant::val_int16() {
        QUINT_ASSERT(get_data_type<int16>() == dt)
        return val_i16;
    }

    uint8 &TypedConstant::val_uint8() {
        QUINT_ASSERT(get_data_type<uint8>() == dt)
        return val_u8;
    }

    uint16 &TypedConstant::val_uint16() {
        QUINT_ASSERT(get_data_type<uint16>() == dt)
        return val_u16;
    }

    uint32 &TypedConstant::val_uint32() {
        QUINT_ASSERT(get_data_type<uint32>() == dt)
        return val_u32;
    }

    uint64 &TypedConstant::val_uint64() {
        QUINT_ASSERT(get_data_type<uint64>() == dt)
        return val_u64;
    }

    int64 TypedConstant::val_int() const {
        QUINT_ASSERT(is_signed(dt));
        if (dt->is_primitive(PrimitiveTypeID::i32)) {
            return val_i32;
        } else if (dt->is_primitive(PrimitiveTypeID::i64)) {
            return val_i64;
        } else if (dt->is_primitive(PrimitiveTypeID::i8)) {
            return val_i8;
        } else if (dt->is_primitive(PrimitiveTypeID::i16)) {
            return val_i16;
        } else {
            QUINT_NOT_IMPLEMENTED
        }
    }

    uint64 TypedConstant::val_uint() const {
        QUINT_ASSERT(is_unsigned(dt));
        if (dt->is_primitive(PrimitiveTypeID::u32)) {
            return val_u32;
        } else if (dt->is_primitive(PrimitiveTypeID::u64)) {
            return val_u64;
        } else if (dt->is_primitive(PrimitiveTypeID::u8)) {
            return val_u8;
        } else if (dt->is_primitive(PrimitiveTypeID::u16)) {
            return val_u16;
        } else {
            QUINT_NOT_IMPLEMENTED
        }
    }

    float64 TypedConstant::val_float() const {
        QUINT_ASSERT(is_real(dt));
        if (dt->is_primitive(PrimitiveTypeID::f32)) {
            return val_f32;
        } else if (dt->is_primitive(PrimitiveTypeID::f64)) {
            return val_f64;
        } else {
            QUINT_NOT_IMPLEMENTED
        }
    }

    int64 TypedConstant::val_as_int64() const {
        if (is_real(dt)) {
            QUINT_ERROR("Cannot cast floating point type {} to int64.", dt->to_string());
        } else if (is_signed(dt)) {
            return val_int();
        } else if (is_unsigned(dt)) {
            return val_uint();
        } else {
            QUINT_NOT_IMPLEMENTED
        }
    }

    float64 TypedConstant::val_cast_to_float64() const {
        if (is_real(dt))
            return val_float();
        else if (is_signed(dt))
            return val_int();
        else if (is_unsigned(dt))
            return val_uint();
        else {
            QUINT_NOT_IMPLEMENTED
        }
    }

}
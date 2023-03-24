//
// Created by BY210033 on 2023/3/10.
//

#ifndef QUINT_TYPE_H
#define QUINT_TYPE_H

#include <string>

#include "common/core.h"

namespace quint::lang {

    enum class PrimitiveTypeID : int {
#define PER_TYPE(x) x,
#include "inc/data_type.inc.h"
#undef PER_TYPE
    };

    class Type {
    public:
        virtual std::string to_string() const = 0;

        template<typename T>
        bool is() const {
            return cast<T>() != nullptr;
        }

        template<typename T>
        const T *cast() const {
            return dynamic_cast<const T *>(this);
        }

        template<typename T>
        T *cast() {
            return dynamic_cast<T *>(this);
        }

        template<typename T>
        T *as() {
            auto p = dynamic_cast<T *>(this);
            QUINT_ASSERT_INFO(p != nullptr, "Cannot treat {} as {}", this->to_string(),
                            typeid(T).name());
            return p;
        }

        template<typename T>
        const T *as() const {
            auto p = dynamic_cast<const T *>(this);
            QUINT_ASSERT_INFO(p != nullptr, "Cannot treat {} as {}", this->to_string(),
                            typeid(T).name());
            return p;
        }

        bool is_primitive(PrimitiveTypeID type) const;

        virtual Type *get_compute_type() {
            QUINT_NOT_IMPLEMENTED
        }

        virtual ~Type() {}

    };

    class DataType {
    public:
        DataType();

        DataType(const Type *ptr) : ptr_((Type *)ptr) {}

        DataType(const DataType &other) : ptr_(other.ptr_) {}

        bool operator==(const DataType &other) const {
            return ptr_ == other.ptr_;
        }

        bool operator!=(const DataType &other) const {
            return !(*this==other);
        }

        std::size_t hash() const;

        std::string to_string() const {
            return ptr_->to_string();
        }

        operator const Type *() const {
            return ptr_;
        }

        operator Type *() {
            return ptr_;
        }

        Type *operator->() const {
            return ptr_;
        }

        DataType &operator=(const DataType &o) {
            ptr_ = o.ptr_;
            return *this;
        }

        bool is_pointer() const;

        void set_is_pointer(bool ptr);

        DataType ptr_removed() const;

        std::vector<int> get_shape() const;

        DataType get_element_type() const;

    private:
        Type *ptr_;
    };

    class PrimitiveType : public Type {
    public:
#define PER_TYPE(x) static DataType x;
#include "inc/data_type.inc.h"
#undef PER_TYPE

        PrimitiveTypeID type;

        explicit PrimitiveType(PrimitiveTypeID type) : type(type) {}

        std::string to_string() const override;

        Type *get_compute_type() override {
            return this;
        }

        static DataType get(PrimitiveTypeID type);

    };

    class PointerType : public Type {
    public:
        PointerType(Type *pointee, bool is_bit_pointer)
            : pointee_(pointee), is_bit_pointer_(is_bit_pointer) {}

        Type *get_pointee_type() const {
            return pointee_;
        }

        auto get_addr_space() const {
            return addr_space_;
        }

        bool is_bit_pointer() const {
            return is_bit_pointer_;
        }

        std::string to_string() const override;

    private:
        Type *pointee_{nullptr};
        int addr_space_{0};
        bool is_bit_pointer_{false};
    };

    class StructType : public Type {
    public:
        explicit StructType(std::vector<const Type *> elements)
            : elements_(std::move(elements)) {}

        Type *get_element_type(const std::vector<int> &indices) const;
        const std::vector<const Type *> &elements() const {
            return elements_;
        }

        int get_num_elements() const {
            int num = 0;
            for (const auto &element : elements_) {
                if (auto struct_type = element->cast<StructType>()) {
                    num += struct_type->get_num_elements();
                }
                // todo handler tensor type
//                else if (auto tensor_type = element->cast<TensorType>()) {
//                    num += tensor_type->get_num_elements();
//                }
                else {
                    QUINT_ASSERT(element->is<PrimitiveType>())
                    num += 1;
                }
            }
            return num;
        }

        std::string to_string() const override;

        Type *get_compute_type() override {
            return this;
        }

    private:
        std::vector<const Type *> elements_;
    };

    class TypedConstant {
    public:
        DataType dt;
        union {
            uint64 value_bits;
            int32 val_i32;
            float32 val_f32;
            int64 val_i64;
            float64 val_f64;
            int8 val_i8;
            int16 val_i16;
            uint8 val_u8;
            uint16 val_u16;
            uint32 val_u32;
            uint64 val_u64;
        };

    public:
        TypedConstant() : dt(PrimitiveType::unknown) {
        }

        explicit TypedConstant(DataType dt) : dt(dt) {
            QUINT_ASSERT_INFO(dt->is<PrimitiveType>(),
                           "TypedConstant can only be PrimitiveType, got {}",
                           dt->to_string());
            value_bits = 0;
        }

        explicit TypedConstant(int32 x) : dt(PrimitiveType::i32), val_i32(x) {
        }

        explicit TypedConstant(float32 x) : dt(PrimitiveType::f32), val_f32(x) {
        }

        explicit TypedConstant(int64 x) : dt(PrimitiveType::i64), val_i64(x) {
        }

        explicit TypedConstant(float64 x) : dt(PrimitiveType::f64), val_f64(x) {
        }

        explicit TypedConstant(int8 x) : dt(PrimitiveType::i8), val_i8(x) {
        }

        explicit TypedConstant(int16 x) : dt(PrimitiveType::i16), val_i16(x) {
        }

        explicit TypedConstant(uint8 x) : dt(PrimitiveType::u8), val_u8(x) {
        }

        explicit TypedConstant(uint16 x) : dt(PrimitiveType::u16), val_u16(x) {
        }

        explicit TypedConstant(uint32 x) : dt(PrimitiveType::u32), val_u32(x) {
        }

        explicit TypedConstant(uint64 x) : dt(PrimitiveType::u64), val_u64(x) {
        }

        template <typename T>
        TypedConstant(DataType dt, const T &value) : dt(dt) {
            // TODO: loud failure on pointers
            dt.set_is_pointer(false);
            if (dt->is_primitive(PrimitiveTypeID::f32)) {
                val_f32 = value;
            } else if (dt->is_primitive(PrimitiveTypeID::i32)) {
                val_i32 = value;
            } else if (dt->is_primitive(PrimitiveTypeID::i64)) {
                val_i64 = value;
            } else if (dt->is_primitive(PrimitiveTypeID::f64)) {
                val_f64 = value;
            } else if (dt->is_primitive(PrimitiveTypeID::i8)) {
                val_i8 = value;
            } else if (dt->is_primitive(PrimitiveTypeID::i16)) {
                val_i16 = value;
            } else if (dt->is_primitive(PrimitiveTypeID::u8)) {
                val_u8 = value;
            } else if (dt->is_primitive(PrimitiveTypeID::u16)) {
                val_u16 = value;
            } else if (dt->is_primitive(PrimitiveTypeID::u32)) {
                val_u32 = value;
            } else if (dt->is_primitive(PrimitiveTypeID::u64)) {
                val_u64 = value;
            } else {
                QUINT_NOT_IMPLEMENTED
            }
        }

        template <typename T>
        bool equal_value(const T &value) const {
            return equal_type_and_value(TypedConstant(dt, value));
        }

        std::string stringify() const;

        bool equal_type_and_value(const TypedConstant &o) const;

        bool operator==(const TypedConstant &o) const {
            return equal_type_and_value(o);
        }

        int32 &val_int32();
        float32 &val_float32();
        int64 &val_int64();
        float64 &val_float64();
        int8 &val_int8();
        int16 &val_int16();
        uint8 &val_uint8();
        uint16 &val_uint16();
        uint32 &val_uint32();
        uint64 &val_uint64();
        int64 val_int() const;
        uint64 val_uint() const;
        float64 val_float() const;
        int64 val_as_int64() const;  // unifies val_int() and val_uint()
        float64 val_cast_to_float64() const;
    };

}

#endif //QUINT_TYPE_H

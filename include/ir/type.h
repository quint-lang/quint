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

}

#endif //QUINT_TYPE_H

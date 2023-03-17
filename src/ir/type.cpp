//
// Created by BY210033 on 2023/3/14.
//
#include "ir/type.h"
#include "ir/type_factory.h"
#include "ir/type_utils.h"

namespace quint::lang {

// This part doesn't look good, but we will remove it soon anyway.
#define PER_TYPE(x)                     \
  DataType PrimitiveType::x = DataType( \
      TypeFactory::get_instance().get_primitive_type(PrimitiveTypeID::x));

#include "inc/data_type.inc.h"
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
#include "inc/data_type.inc.h"
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

}
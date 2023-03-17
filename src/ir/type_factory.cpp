//
// Created by BY210033 on 2023/3/14.
//
#include "ir/type_factory.h"
#include "ir/type_utils.h"

namespace quint::lang {

    TypeFactory &TypeFactory::get_instance() {
        static TypeFactory *type_factory = new TypeFactory;
        return *type_factory;
    }

    Type *TypeFactory::get_primitive_type(PrimitiveTypeID id) {
        std::lock_guard<std::mutex> _(primitive_mut_);

        if (primitive_types_.find(id) == primitive_types_.end()) {
            primitive_types_[id] = std::make_unique<PrimitiveType>(id);
        }
        return primitive_types_[id].get();
    }

    PrimitiveType *TypeFactory::get_primitive_int_type(int bits, bool is_signed) {
        Type *int_type;
        if (bits == 8) {
            int_type = get_primitive_type(PrimitiveTypeID::i8);
        } else if (bits == 16) {
            int_type = get_primitive_type(PrimitiveTypeID::i16);
        } else if (bits == 32) {
            int_type = get_primitive_type(PrimitiveTypeID::i32);
        } else if (bits == 64) {
            int_type = get_primitive_type(PrimitiveTypeID::i64);
        } else {
            QUINT_ERROR("No primitive int type has {} bits", bits)
        }
        if (!is_signed) {
            int_type = to_unsigned(DataType(int_type));
        }
        return int_type->cast<PrimitiveType>();
    }

    Type *TypeFactory::get_pointer_type(Type *element, bool is_bit_pointer) {
        std::lock_guard<std::mutex> _(pointer_mut_);

        auto key = std::make_pair(element, is_bit_pointer);
        if (pointer_types_.find(key) == pointer_types_.end()) {
            pointer_types_[key] =
                    std::make_unique<PointerType>(element, is_bit_pointer);
        }
        return pointer_types_[key].get();
    }

    Type *TypeFactory::get_struct_type(const std::vector<const Type *> &elements) {
        std::lock_guard<std::mutex> _(struct_mut_);

        if (struct_types_.find(elements) == struct_types_.end()) {
            for (const auto &element : elements) {
                QUINT_ASSERT_INFO(
                        element->is<PrimitiveType>() || element->is<StructType>() ||
                                element->is<PointerType>(),
                        "Unsupported struct element type: " +element->to_string())
            }
            struct_types_[elements] = std::make_unique<StructType>(elements);
        }
        return struct_types_[elements].get();
    }

}
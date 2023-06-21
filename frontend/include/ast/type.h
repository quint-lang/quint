//
// Created by BY210033 on 2023/5/31.
//

#ifndef QUINT_TYPE_H
#define QUINT_TYPE_H

#include "util/cast.h"

#include <utility>
#include <vector>
#include <cassert>
#include <algorithm>

namespace quint::ast {

    class Type : public QuintObj {
    public:
        enum TypeKind {
            Bool,
            Tuple,
            Array,
            String,
            Qubit,
            Int,
            Double,
            Uint,
            Map,
            Func,
            Complex,
            Circuit
        };

        Type(): kind(Bool) {}

        Type(TypeKind kind): kind(kind) {}

        TypeKind getKind() const { return kind; }

        virtual bool operator==(Type &other) const { return false; }

        virtual std::string toString() const {
            return "";
        }

        virtual bool classof(const type_info &info) const override {
            return false;
        }

        virtual size_t toHash() const {
            return 0;
        }

        virtual bool isClassical() { return false; }

        virtual bool hasClassicalComponent() { return false; }

        virtual Type* copyImpl() { return nullptr; }

        friend std::ostream &operator<<(std::ostream &os, const Type &obj) {
            return os << obj.toString();
        }

    private:
        TypeKind kind;
    };

}

namespace std {
    template<>
    struct hash<quint::ast::Type> {
        std::size_t operator()(const quint::ast::Type &t) const {
            return t.toHash();
        }
    };
}

namespace quint::ast {

    class BoolTy : public Type {
        bool classical;
    public:
        explicit BoolTy(bool classical) : classical(classical), Type(Bool) {}

        bool operator==(Type& other) const override {
            if (isa<BoolTy>(other)) {
                auto *p = static_cast<BoolTy*>(&other);
                return p->classical == classical;
            }
            return false;
        }

        bool isClassical() override {
            return classical;
        }

        Type* copyImpl() override {
            return this;
        }

        std::string toString() const override {
            if (classical) {
                return "bool";
            }
            return "boo<1>";
        }

        bool hasClassicalComponent() override {
            return classical;
        }

        bool classof(const type_info &info) const override {
            return info == typeid(BoolTy);
        }

        size_t toHash() const override {
            return std::hash<bool>()(classical) + 0x9e3779b9;
        }
    };

    class TupleTy : public Type {
        std::vector<std::shared_ptr<Type>> types;
    public:
        explicit TupleTy(const std::vector<std::shared_ptr<Type>>& types)
            : types(types), Type(Tuple) {}

        size_t length() {
            return types.size();
        }

        bool operator==(Type &other) const override {
            if (isa<TupleTy>(other)) {
                auto *p = static_cast<TupleTy*>(&other);
                if (types.size() == p->types.size()) {
                    return std::equal(types.begin(), types.end(), p->types.begin());
                }
                return false;
            }
            return false;
        }

        std::string toString() const override {
            std::string res = "[" ;
            for (auto &ty : types) {
                res += ty->toString() + ", ";
            }
            res = res.substr(0, res.length()-3);
            return res + "]";
        }

        bool classof(const type_info &info) const override {
            return info == typeid(TupleTy);
        }

        bool isClassical() override {
            return std::all_of(types.begin(), types.end(), [](auto x) {
                return x->isClassical();
            });
        }

        bool hasClassicalComponent() override {
            return std::any_of(types.begin(), types.end(), [](auto x) {
                return x->isClassical();
            });
        }

        Type* copyImpl() override {
            return this;
        }

        size_t toHash() const override {
            size_t hash_value = 0x9e3779bb;
            for (const auto& ty : types) {
                if (ty) {
                    hash_value ^= std::hash<Type>()(*ty) + 0x9e3779bb +(hash_value << 6) +(hash_value >> 2);
                }
            }
            return hash_value;
        }

    };

    class ArrayTy : public Type {
        std::shared_ptr<Type> next;
    public:
        explicit ArrayTy(std::shared_ptr<Type>  next): next(std::move(next)), Type(Array) {}

        bool operator==(Type &other) const override {
            if (isa<ArrayTy>(other)) {
                auto p = static_cast<ArrayTy*>(&other);
                return next == p->next;
            }
            return Type::operator==(other);
        }

        std::string toString() const override {
            return next->toString() + "[]";
        }

        bool classof(const type_info &info) const override {
            return info == typeid(ArrayTy);
        }

        bool isClassical() override {
            return next->isClassical();
        }

        bool hasClassicalComponent() override {
            return next->isClassical();
        }

        Type* copyImpl() override {
            return this;
        }

        size_t toHash() const override {
            size_t hash_value = 0x9e3779bd;
            if (next) {
                hash_value ^= std::hash<Type>()(*next) + 0x9e3779bd + (hash_value << 6) + (hash_value >> 2);
            }
            return hash_value;
        }
    };

    class StringTy : public Type {
        bool classical;
    public:
        StringTy(bool classical): classical(classical), Type(String) {}

        bool operator==(Type &other) const override {
            if (isa<StringTy>(other)) {
                return true;
            }
            return false;
        }

        std::string toString() const override {
            return "string";
        }

        bool classof(const type_info &info) const override {
            return info == typeid(StringTy);
        }

        bool isClassical() override {
            return classical;
        }

        bool hasClassicalComponent() override {
            return classical;
        }

        Type* copyImpl() override {
            return this;
        }

        size_t toHash() const override {
            return std::hash<bool>()(classical) + 0x9e3779bf;
        }
    };

    class QubitTy : public Type {
    public:
        QubitTy() : Type(Qubit) {}

    private:
        bool operator==(Type &other) const override {
            if (isa<QubitTy>(other)) {
                return true;
            }
            return false;
        }

        std::string toString() const override {
            return "qubit";
        }

        bool classof(const type_info &info) const override {
            return info == typeid(QubitTy);
        }

        bool isClassical() override {
            return false;
        }

        bool hasClassicalComponent() override {
            return false;
        }

        Type* copyImpl() override {
            return this;
        }
    };

    class IntTy : public Type {
        int size;
        bool classical;
    public:
        explicit IntTy(bool classical = false, int size = 0): size(size), classical(classical), Type(Int) {}

        bool operator==(Type &other) const override {
            if (isa<IntTy>(other)) {
                auto *p = static_cast<IntTy*>(&other);
                if (p->classical == classical) {
                    if (classical) {
                        return true;
                    }
                    if (p->size == size) {
                        return true;
                    }
                }
            }
            return false;
        }

        std::string toString() const override {
            if (classical) {
                return "int";
            }
            return "int<" + std::to_string(size) + ">";
        }

        bool classof(const type_info &info) const override {
            return info == typeid(IntTy);
        }

        bool isClassical() override {
            return classical;
        }

        bool hasClassicalComponent() override {
            return classical;
        }

        Type* copyImpl() override {
            return this;
        }

        size_t toHash() const override {
            size_t hash_value = 0;
            hash_value ^= std::hash<bool>()(classical) + 0x9e3779c3 + (hash_value << 6) + (hash_value >> 2);
            hash_value ^= std::hash<int>()(size) + 0x9e3779c3 + (hash_value << 6) + (hash_value >> 2);
            return hash_value;
        }
    };

    class DoubleTy : public Type {
    public:
        DoubleTy(): Type(Double) {}

        bool operator==(Type &other) const override {
            if (isa<DoubleTy>(other)) {
                return true;
            }
            return false;
        }

        std::string toString() const override {
            return "double";
        }

        bool classof(const type_info &info) const override {
            return info == typeid(DoubleTy);
        }

        bool isClassical() override {
            return true;
        }

        bool hasClassicalComponent() override {
            return true;
        }

        Type* copyImpl() override {
            return this;
        }

        size_t toHash() const override {
            return 0x9e3779c5;
        }
    };

    class UintTy : public Type {
        int size;
        bool classical;
    public:
        explicit UintTy(bool classical = false, int size = 0): classical(classical), size(size), Type(Uint) {}

        bool operator==(Type &other) const override {
            if (isa<UintTy>(other)) {
                auto *p = static_cast<UintTy*>(&other);
                if (p->classical == classical) {
                    if (classical) {
                        return true;
                    }
                    if (p->size == size) {
                        return true;
                    }
                }
            }
            return Type::operator==(other);
        }

        std::string toString() const override {
            if (classical) {
                return "uint";
            }
            return "uint<" + std::to_string(size) + ">";
        }

        bool classof(const type_info &info) const override {
            return info == typeid(UintTy);
        }

        bool isClassical() override {
            return classical;
        }

        bool hasClassicalComponent() override {
            return classical;
        }

        Type* copyImpl() override {
            return this;
        }

        size_t toHash() const override {
            size_t hash_value = 0;
            hash_value ^= std::hash<bool>()(classical) + 0x9e3779c7 + (hash_value << 6) + (hash_value >> 2);
            hash_value ^= std::hash<int>()(size) + 0x9e3779c7 + (hash_value << 6) + (hash_value >> 2);
            return hash_value;
        }
    };

    class ClosureTy : public Type {
    public:
        std::vector<std::unique_ptr<Type>> args;
        std::unique_ptr<Type> ret;
        bool classical;

        ClosureTy(std::vector<std::unique_ptr<Type>> args, std::unique_ptr<Type> ret)
            : args(std::move(args)), ret(std::move(ret)), Type(Func) {}

        bool operator==(Type &other) const override {
            return Type::operator==(other);
        }

        std::string toString() const override {
            return "closure";
        }

        bool classof(const type_info &info) const override {
            return info == typeid(ClosureTy);
        }

        size_t toHash() const override {
            return Type::toHash();
        }

        bool isClassical() override {
            return classical;
        }

        bool hasClassicalComponent() override {
            return classical;
        }

        Type* copyImpl() override {
            return this;
        }
    };

    class MapTy : public Type {
        Type key;
        Type value;

    };

    class TypeTy : public Type {

    };

    bool isInt(ast::Type &ty);

    bool isUint(ast::Type &ty);

}


#endif //QUINT_TYPE_H

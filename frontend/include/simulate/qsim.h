//
// Created by BY210033 on 2023/5/18.
//

#ifndef QUINT_QSIM_H
#define QUINT_QSIM_H

#include "simulate/hashtable.h"
#include "ast/expr.h"
#include "ast/type.h"
#include "structure/package.h"

#include <map>
#include <tuple>
#include <unordered_map>
#include <complex>
#include <functional>

namespace quint {

    class QState;

    class Sigma;

    class QVar;

    class Value;

    template<typename... Args>
    using Q = std::tuple<Args...>;

    using Record = std::unordered_map<std::string, Value>;

    struct FormattingOptions {
        enum FormattingType {
            default_,
            dump,
        };
        FormattingType type;

        FormattingOptions() : type(default_) {}

        FormattingOptions(FormattingType type) : type(type) {}
    };

    std::string formatQValue(QState qs, Value value);

    long smallValue(size_t value);

    struct HadamardUnitary {
        HadamardUnitary() {}

        void operator()(std::function<void(Value, std::complex<double>)> func, Value x);
    };

    struct XUnitary {
        void operator()(std::function<void(Value, std::complex<double>)> func, Value x);
    };

    struct YUnitary {
        void operator()(std::function<void(Value, std::complex<double>)> func, Value x);
    };

    struct ZUnitary {
        void operator()(std::function<void(Value, std::complex<double>)> func, Value x);
    };

    struct RotX {
        void operator()(std::function<void(Value, std::complex<double>)> func, Value x, double phi);
    };

    struct RotY {
        void operator()(std::function<void(Value, std::complex<double>)> func, Value x, double phi);
    };

    struct RotZ {
        void operator()(std::function<void(Value, std::complex<double>)> func, Value x, double phi);
    };

    class QVal : public std::enable_shared_from_this<QVal> {
    public:
        virtual std::string toString() const { return "_(" + std::string(typeid(this).name()) + ")"; }

        friend std::ostream &operator<<(std::ostream &os, const QVal &val) {
            return os << val.toString();
        }

        virtual Value get(Sigma &);

        virtual QVar dup(QState &state, Value self);

        virtual std::shared_ptr<QVal> consumeOnRead() {
            return shared_from_this();
        }

        virtual void forget(QState &state, Value rhs);

        virtual void forget(QState &state);

        virtual bool equals(QVal &other);

        virtual Value toVar(QState &state, Value self, bool cleanUp);

        virtual void removeVar(Sigma &sigma);

        virtual size_t toHash() const { return 0; }

        template<typename unitary, typename... T>
        Value applyUnitary(QState &qs, ast::Type type, T... controls);

        template<typename unitary, typename... T>
        static Value applyUnitaryToClassical(QState &qs, Value value, ast::Type type, T... controls);

        bool operator==(QVal &other) {
            return equals(other);
        }
    };

}

namespace std {
    template<>
    struct hash<quint::QVal> {
        std::size_t operator()(const quint::QVal &t) const {
            return t.toHash();
        }
    };
}

namespace quint {

    class QConst : public QVal {
        std::shared_ptr<Value> constant;
    public:
        QConst(std::shared_ptr<Value> constant);

        std::string toString() const override;

        Value get(Sigma &sigma) override;

        bool equals(QVal &other) override;

        size_t toHash() const override;
    };

    class QVar : public QVal {
        size_t ref_;
        bool consumedOnRead = false;
    public:
        QVar(size_t ref_) : ref_(ref_) {}

        std::string toString() const override;

        Value get(Sigma &sigma) override;

        QVar dup(QState &state, Value self) override;

        std::shared_ptr<QVal> consumeOnRead() override;

        void forget(QState &state, Value rhs) override;

        void forget(QState &state) override;

        Value toVar(QState &state, Value self, bool cleanUp) override;

        void removeVar(Sigma &sigma) override;

        void assign(QState &state, Value rhs);

        bool equals(QVal &other) override;

        size_t toHash() const override;
    };

    class ConvertQVal : public QVal {
        std::shared_ptr<Value> value;
        std::shared_ptr<ast::Type> ntype;
    public:
        ConvertQVal(std::shared_ptr<Value> value, std::shared_ptr<ast::Type> ntype);

        Value get(Sigma &sigma) override;

        void forget(QState &state, Value rhs) override;

        void forget(QState &state) override;

        void removeVar(Sigma &sigma) override;

        bool equals(QVal &other) override;

        size_t toHash() const override;
    };

    class IndexQVal : public QVal {
        std::shared_ptr<Value> value, i;
    public:
        IndexQVal(std::shared_ptr<Value> value, std::shared_ptr<Value> i);

        Value get(Sigma &sigma) override;

        bool equals(QVal &other) override;

        size_t toHash() const override;
    };

    class Value {
    public:
        std::unique_ptr<ast::Type> type;
        std::unique_ptr<ast::Expr> expression;

        enum Tag {
            unknown,
            array,
            fval,
            qval,
            intval,
            uintval,
            doubleval,
            bval,
            sval,
            record,
        };

        Value();

        Value(const Value &rhs);

        static Tag getTag(const std::unique_ptr<ast::Type> &type_) {
            assert(type_ != nullptr);
            switch (type_->getKind()) {
                case ast::Type::Tuple:
                case ast::Type::Array:
                case ast::Type::Map:
                    return array;
                case ast::Type::Func:
                    return fval;
                case ast::Type::Qubit:
                    return qval;
                case ast::Type::Int:
                    return intval;
                case ast::Type::Uint:
                    return uintval;
                case ast::Type::Double:
                    return doubleval;
                case ast::Type::Bool:
                    return bval;
                case ast::Type::String:
                    return sval;
                default:
                    return unknown;
            }
        }

        union {
            std::vector<Value> array_;
            double doubleval_;
            std::shared_ptr<QVal> qval_;
            BitInt<true> intval_;
            BitInt<false> uintval_;
            std::string sval_;
            Record record_;
            bool bval_;
        };

        std::variant<std::vector<Value>, double, QVal, BitInt<true>, BitInt<false>, Record, std::string, bool> data;

        Tag tag() {
            return getTag(type);
        }

        bool isValid() {
            return type != nullptr;
        }

        Value &operator=(Value rhs);

        Value dup(QState &state);

        Value toVar(QState &state, bool cleanUp);

        void assign(QState &state, Value &rhs);

        size_t to_hash() const;

        void forget(QState &state, Value rhs);

        void forget(QState &state);

        void removeVar(Sigma &sigma);

        Value consumeOnRead();

        template<typename unitary, typename... T>
        Value applyUnitary(QState &qs, ast::Type type, T... controls);

        bool isClassical();

        Value convertTo(ast::Type ntype);

        Value inFrame() {
            return *this;
        }

        Value operator[](size_t i);

        Value operator[](Value i);

        Value slice(size_t l, size_t r);

        Value slice(Value l, Value r);

        Value operator-();

        Value operator~();

        Value operator!();

        Value binaryType(std::string op, ast::Expr t1, ast::Expr t2);

        Value operator+(Value &r);

        Value operator-(Value &r);

        Value operator*(Value &r);

        Value operator/(Value &r);

        Value operator%(Value &r);

        Value operator|(Value &r);

        Value operator^(Value &r);

        Value operator&(Value &r);

        Value operator<<(Value &r);

        Value operator>>(Value &r);

        Value pow(Value &r);

        Value eqZ();

        bool eqZImpl();

        Value neqZ();

        bool negZImpl();

        bool operator==(Value &r);

        bool operator>(Value &r);

        bool operator<(Value &r);

        bool operator!=(Value &r);

        bool operator>=(Value &r);

        bool operator<=(Value &R);

        Value sqrt();

        Value sin();

        Value exp();

        Value log();

        Value asin();

        Value cos();

        Value acos();

        Value tan();

        Value atan();

        Value classicalValue(Sigma state);

        bool asBoolean();

        bool isZ();

        size_t asZ();

        bool isQ();

        std::string toStringImpl(FormattingOptions opt);

        std::string toBasicStringImpl();

        std::string toString();

        virtual ~Value();
    };
} // namespace quint

namespace std {
    template<>
    struct hash<quint::Value> {
        std::size_t operator()(const quint::Value &t) const {
            return t.to_hash();
        }
    };
}

namespace quint {

    Value makeTuple(ast::Type type, std::vector<Value> tuple);

    Value makeArray(ast::Type type, std::vector<Value> array);

    Value makeVector(ast::Type type, std::vector<Value> vector);

    Value makeRecord(ast::Type, Record record);

//    Value makeFunc()

    Value makeQval(ast::Type &type, std::shared_ptr<QVal> qval);

//    Value makeRotation()

    Value makeInteger(size_t value);

    Value makeInt(ast::Type type, BitInt<true> value);

    Value makeUint(ast::Type type, BitInt<false> value);

    Value makeBool(bool value);

    Value nullValue() {
        return {};
    }

    Value typeValue();

    Value pi();

    size_t fnv(size_t data, size_t start) {
        size_t fnvp, fnvb;
        if (sizeof(size_t) == 8) {
            fnvp = 1099511628211LU;
            fnvb = 14695981039346656037LU;
        } else {
            fnvp = 16777619U;
            fnvb = 2166136261U;
        }
        return (start^data)*fnvp;
    }

    class Sigma {
    public:
        typedef size_t Ref;
        std::unordered_map<Ref, Value> vars;
    public:
        static Ref curRef;

        Sigma dup() const;

        Ref assign(Ref ref_, Value v);

        void forget(Ref ref_);

        void forget(Ref ref_, Value v);

        size_t to_hash() const;

        void relabel(std::map<Ref, Ref> relabeling);

        Value getVar(Ref ref_) {
            if (vars.count(ref_)) {
                return vars.at(ref_);
            }
            return {};
        }

        bool operator==(const Sigma &other) const {
            return true;
        }

        struct Sortable {
            std::vector<Q<Ref, Value>> values;
        private:
            static int cmp(Value a, Value b) {
                assert(a.type == b.type);
//                if ()
            }

        public:
            int onCmp(Sortable rhs) {
                return 0;
            }

            std::string toStringImpl(FormattingOptions opt) {
                if (values.empty()) {
                    return "|" + std::string(u8"\u27E9");
                }
                // todo finish
                return "";
            }
        };

        Sortable toSortable();

        std::string toStringImpl(FormattingOptions opt) {
            return toSortable().toStringImpl(opt);
        }

        std::string toString();
    };

} // namespace quint

namespace std {
    template<>
    struct hash<quint::Sigma> {

        std::size_t operator()(const quint::Sigma& s) const {
            return s.to_hash();
        }
    };
}

namespace quint {

    class QState {
    public:
        std::unordered_map<Sigma, std::complex<double>> state;
        Record vars;
        std::vector<std::shared_ptr<QVal>> popFrameCleanup;

        static Value dupValue(Value v);

        static std::vector<Value> dupValue(std::vector<Value> r);

        static Record dupValue(Record r);

        std::string toString();

        void dump();

        QState dup();

        void copyNonState(QState &rhs);

        void add(Sigma k, std::complex<double> v);

        void updateRelabeling(std::map<size_t, size_t> relabeling, Value to, Value from);

        void operator+=(QState &r);

        Q<QState, QState> split(Value cond);

        template<typename F, bool checkInterference = true, typename... T>
        QState map(T... args);

        static QState empty() {
            return {};
        }

        static QState unit();

        QState pushFrame();

        QState popFrame();

        static Value inFrame(Value v) {
            return v.inFrame();
        }

        void passParameter(std::string prm, bool isConst, Value rhs);

        // todo finish ast
        void passContext();

        // todo finish ast
        Value call();

        // todo finish ast
        Value call(Value fun, Value arg, ast::Type type);

        QState assertTrue(Value val);

        Value readLocal(std::string s, bool constLookup);

        // todo finish ast
        Value makeFunction();

        // todo finsh ast
        // Value makeFunction();

        // todo finish ast
        void declareFunction();

        static Value ite(Value cond, Value then, Value othw);

        Value makeQVar(Value v);

        void forget(size_t var, Value rhs);

        void forget(size_t var);

        // todo finish ast
//        void forgetVars();

        void assignTo(size_t var, Value rhs);

        void assignTo(Value &var, Value rhs);

        void catAssignTo(Value &var, Value rhs);

        void assignTo(std::string lhs, Value rhs);

        void castAssignTo(std::string lhs, Value rhs);

        Value H(Value x);

        Value X(Value x);

        Value Y(Value x);

        Value Z(Value x);

        Value phase(Value phi);

        Value rX(Value args);

        Value rY(Value args);

        Value rZ(Value args);

        Value array_(ast::Type type, Value arg);

        Value reverse(ast::Type, Value arg);

        Value measure(Value arg);

        virtual ~QState() {}
    };

    // todo finish ast
    template<typename QS>
    Value readVariable(QS &qs);

    class QSim {
        std::string sourceFile;
    public:
        QSim(std::string sourceFile) : sourceFile(sourceFile) {}
    };

}

#endif //QUINT_QSIM_H

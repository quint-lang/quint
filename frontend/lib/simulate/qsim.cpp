//
// Created by BY210033 on 2023/5/18.
//
#include "simulate/qsim.h"

namespace quint {

    Value QVal::get(Sigma &) { return {}; }

    QVar QVal::dup(QState &state, Value self) {
        auto nref_ = Sigma::curRef++;
        state.assignTo(nref_, self);
        return {nref_};
    }

    Value QVal::toVar(QState &state, Value self, bool cleanUp) {
        auto r = state.makeQVar(self);
        if (cleanUp) {
            auto var = std::dynamic_pointer_cast<QVar>(r.qval_);
            assert(var != nullptr);
            state.popFrameCleanup.push_back(var);
        }
        return r;
    }

    void QVal::removeVar(Sigma &sigma) {}

    void QVal::forget(QState &state, Value rhs) {}

    void QVal::forget(QState &state) {}

    bool QVal::equals(QVal &other) {
        return false;
    }

    template<typename unitary, typename... T>
    Value QVal::applyUnitary(QState &qs, ast::Type type, T... controls) {
        QState nstate;
        nstate.copyNonState(qs);
        auto ref_ = Sigma::curRef++;
        for (auto &pair : qs.state) {
            auto func_add = [&](Value nk, std::complex<double> nv) {
                auto sigma = pair.first.dup();
                sigma.assign(ref_, nk);
                removeVar(sigma);
                nstate.add(sigma, nv * pair.second);
            };
            auto key = pair.first;
            unitary()(func_add, get(key), controls...);
        }
        auto var = std::make_shared<QVar>(ref_);
        auto r = makeQval(type, var);
        qs = nstate;
        return r;
    }

    template<typename unitary, typename... T>
    Value QVal::applyUnitaryToClassical(QState &qs, Value value, ast::Type type, T... controls) {
        QState nstate;
        nstate.copyNonState(qs);
        auto ref_ = Sigma::curRef++;
        for (auto &pair : qs.state) {
            auto func_add = [&](Value nk, std::complex<double> nv) {
                auto sigma = pair.first.dup();
                sigma.assign(ref_, nk);
                nstate.add(sigma, nv * pair.second);
            };
            unitary()(func_add, value, controls...);
        }
        auto var = std::make_shared<QVar>(ref_);
        auto r = makeQval(type, var);
        qs = nstate;
        return r;
    }

    Value::Value() {

    }

    Value::Value(const Value &rhs) {
        operator=(rhs);
    }

    Value& Value::operator=(Value rhs) {
        *type = *rhs.type;
        if (type == nullptr)
            return *this;

        // todo Complete the other types
        switch (tag()) {
            case array:
                data = std::get<std::vector<Value>>(rhs.data);
                array_ = rhs.array_;
                break;
            case qval:
                data = std::get<QVal>(rhs.data);
                qval_ = rhs.qval_;
                break;
            case intval:
                data = std::get<BitInt<true>>(rhs.data);
                intval_ = rhs.intval_;
                break;
            case uintval:
                data = std::get<BitInt<false>>(rhs.data);
                uintval_ = rhs.uintval_;
                break;
            case doubleval:
                data = std::get<double>(rhs.data);
                doubleval_ = rhs.doubleval_;
                break;
            case bval:
                data = std::get<bool>(rhs.data);
                bval_ = rhs.bval_;
                break;
            case sval:
                data = std::get<std::string>(rhs.data);
                sval_ = rhs.sval_;
                break;
            case record:
                data= std::get<Record>(rhs.data);
                record_ = rhs.record_;
                break;
            default:
                break;
        }
        return *this;
    }

    Value Value::dup(QState &state) {
        Record nrecord;
        std::vector<Value> arrayCopy;
        switch (tag()) {
            case intval:
            case uintval:
            case doubleval:
            case bval:
            case sval:
                return *this;
            case record:
                for (auto &pair : record_) {
                    nrecord[pair.first] = pair.second.dup(state);
                }
                return makeRecord(*type, nrecord);
            case array:
                std::transform(array_.begin(), array_.end(), std::back_inserter(arrayCopy), [&](auto& x) {
                    return x.dup(state);
                });
                return makeArray(*type, arrayCopy);
            case qval:
                return makeQval(*type, std::make_shared<QVar>(qval_->dup(state, *this)));
            default:
                return *this;
        }
    }

    Value Value::toVar(QState &state, bool cleanUp) {
        Record nrecord;
        std::vector<Value> arrayCopy;
        if (isClassical())
            return *this;
        switch (tag()) {
            case intval:
            case uintval:
            case doubleval:
            case bval:
            case sval:
                assert(0);
            case array:
                std::transform(array_.begin(), array_.end(), std::back_inserter(arrayCopy), [&](auto &v) {
                    return v.toVar(state, cleanUp);
                });
                return makeArray(*type, arrayCopy);
            case record:
                for (auto &pair : record_) {
                    nrecord[pair.first] = pair.second.toVar(state, cleanUp);
                }
                return makeRecord(*type, nrecord);
            case qval:
                return qval_->toVar(state, *this, cleanUp);
            default:
                assert(0);
        }
        return Value();
    }

    void Value::assign(QState &state, Value &rhs) {
        bool ok;
        if (!type) {
            *this = rhs;
            return;
        }
        if (isClassical()) {
            if (rhs.isClassical())
                *this = rhs.dup(state);
            else
                *this = rhs.toVar(state, false);
            return;
        }
        if (rhs.isClassical()) {
            Value nrhs;
            *nrhs.type = *type;
            nrhs.qval_ = std::make_shared<QConst>(std::make_shared<Value>(rhs));
        }
        assert(tag() == rhs.tag());
        switch (tag()) {
            case intval:
            case uintval:
            case doubleval:
            case bval:
            case sval:
                *this = rhs;
                break;
            case array:
                assert(rhs.tag() == array);
                if (array_.size() == rhs.array_.size()) {
                    for (size_t i = 0; i < array_.size(); ++i) {
                        array_[i].assign(state, rhs.array_[i]);
                    }
                } else {
                    forget(state);
                    *this = rhs.dup(state);
                }
                return;
            case record:
                assert(rhs.tag() == record);
                ok = true;
                for (auto &pair : rhs.record_) {
                    if (!record_.count(pair.first))
                        ok = false;
                }
                for (auto &pair : record_) {
                    if (!rhs.record_.count(pair.first))
                        ok = false;
                }
                if (ok) {
                    for (auto &pair : record_) {
                        pair.second.assign(state, rhs.record_[pair.first]);
                    }
                } else {
                    forget(state);
                    *this = rhs.dup(state);
                }
                return;
            case qval:
                if (auto quvar = std::dynamic_pointer_cast<QVar>(qval_))
                    quvar->assign(state, rhs);
                break;
            default:
                break;
        }
    }

    size_t Value::to_hash() const {
        size_t value = std::hash<ast::Type>()(*type);
        auto flag = type->getKind();
        if (flag == ast::Type::Func) {
            value ^= 1;
        } else if (flag == ast::Type::Map) {
            value ^= 2;
        } else if (flag == ast::Type::Array) {
            for (auto &item : array_) {
                value ^= std::hash<Value>()(item) + 0x9e3779bd + (value << 6) + (value >> 2);
            }
        } else if (!type->isClassical()) {
            value ^= std::hash<QVal>()(*qval_) + 0x9e3779c1 + (value << 6) + (value >> 2);
        } else if (flag == ast::Type::Double) {
            value ^= std::hash<double>()(doubleval_) + 0x9e3779c5 + (value << 6) + (value >> 2);
        } else if (flag == ast::Type::Bool) {
            value ^= std::hash<bool>()(bval_) + 0x9e3779b9 + (value << 6) + (value >> 2);
        } else if (flag == ast::Type::String) {
            value ^= std::hash<std::string>()(sval_) + 0x9e3779bf + (value << 6) + (value >> 2);
        } else if (flag == ast::Type::Uint) {
            value ^= std::hash<BitInt<false>>()(uintval_) + 0x9e3779c7 + (value << 6) + (value >> 2);
        } else if (flag == ast::Type::Int) {
            value ^= std::hash<BitInt<true>>()(intval_) + 0x9e3779c3 + (value << 6) + (value >> 2);
        }
        return value;
    }

    void Value::removeVar(Sigma &sigma) {
        switch (tag()) {
            case intval:
            case uintval:
            case doubleval:
            case bval:
            case sval:
                assert(isClassical());
                return;
            case array:
                for (auto x : array_) {
                    x.removeVar(sigma);
                }
                return;
            case record:
                for (auto &pair : record_) {
                    pair.second.removeVar(sigma);
                }
                return;
            case qval:
                qval_->removeVar(sigma);
                return;
            default:
                return;
        }
    }

    void Value::forget(QState &state, Value rhs) {
        switch (tag()) {
            case intval:
            case uintval:
            case doubleval:
            case bval:
            case sval:
                assert(isClassical());
                return;
            case array:
                assert(rhs.tag() == array);
                assert(array_.size() == rhs.array_.size());
                for (size_t i = 0; i < array_.size(); ++i) {
                    array_[i].forget(state, rhs.array_[i]);
                }
                return;
            case record:
                assert(rhs.tag() == record);
                for (auto &pair : rhs.record_) {
                    assert(record_.count(pair.first));
                }
                for (auto &pair : record_) {
                    pair.second.forget(state, rhs.record_[pair.first]);
                }
                return;
            case qval:
                qval_->forget(state, rhs);
                return;
            default:
                return;
        }
    }

    void Value::forget(QState &state) {
        switch (tag()) {
            case intval:
            case uintval:
            case doubleval:
            case bval:
            case sval:
                assert(isClassical());
                return;
            case array:
                for (auto x : array_) {
                    x.forget(state);
                }
                return;
            case record:
                for (auto &pair : record_) {
                    pair.second.forget(state);
                }
                return;
            case qval:
                qval_->forget(state);
                return;
            default:
                return;
        }
    }

    Value Value::consumeOnRead() {
        Record nrecord;
        std::vector<Value> copy;
        switch (tag()) {
            case intval:
            case uintval:
            case doubleval:
            case bval:
            case sval:
                return *this;
            case record:
                for (auto &pair : record_) {
                    nrecord[pair.first] = pair.second.consumeOnRead();
                }
                return makeRecord(*type, nrecord);
            case array:
                std::transform(array_.begin(), array_.end(), std::back_inserter(copy), [&](auto& x) {
                    return x.consumeOnRead();
                });
                return makeArray(*type, copy);
            case qval:
                return makeQval(*type, qval_->consumeOnRead());
            default:
                return *this;
        }
    }

    template<typename unitary, typename... T>
    Value Value::applyUnitary(QState &qs, ast::Type type, T... controls) {
        if (isClassical())
            return QVal::applyUnitaryToClassical<unitary>(qs, *this, type, controls...);
        assert(tag() == qval);
        return qval_->applyUnitary<unitary>(qs, type, controls...);
    }

    bool Value::isClassical() {
        switch (tag()) {
            case intval:
            case uintval:
            case doubleval:
            case bval:
            case sval:
                return true;
            case array:
                return std::all_of(array_.begin(), array_.end(), [](Value &x) {
                    return x.isClassical();
                });
            case record:
                for (auto &pair : record_) {
                    if (!pair.second.isClassical()) {
                        return false;
                    }
                }
                return true;
            case qval:
                return false;
            default:
                return true;
        }
        return false;
    }

    // todo finish ast
    Value Value::convertTo(ast::Type ntype) {
        return Value();
    }

    Value Value::operator[](size_t i) {
        auto tag_ = tag();
        assert(tag_ == array || isUint(*type)|| isInt(*type));
        switch (tag_) {
            case array:
                assert(i >= 0 && i < array_.size());
                return array_[i];
            case qval:
                assert(tag() == uintval || tag() == intval);

        }
        return Value();
    }

    Value Value::operator[](Value i) {
        return Value();
    }

    Value Value::slice(size_t l, size_t r) {
        return Value();
    }

    Value Value::slice(Value l, Value r) {
        return Value();
    }

    Value Value::operator-() {
        return Value();
    }

    Value Value::operator~() {
        return Value();
    }

    Value Value::operator!() {
        return Value();
    }

    Value Value::binaryType(std::string op, ast::Expr t1, ast::Expr t2) {
        return Value();
    }

    Value Value::operator+(Value &r) {
        return Value();
    }

    Value Value::operator-(Value &r) {
        return Value();
    }

    Value Value::operator*(Value &r) {
        return Value();
    }

    Value Value::operator/(Value &r) {
        return Value();
    }

    Value Value::operator%(Value &r) {
        return Value();
    }

    Value Value::operator|(Value &r) {
        return Value();
    }

    Value Value::operator^(Value &r) {
        return Value();
    }

    Value Value::operator&(Value &r) {
        return Value();
    }

    Value Value::operator<<(Value &r) {
        return Value();
    }

    Value Value::operator>>(Value &r) {
        return Value();
    }

    Value Value::pow(Value &r) {
        return Value();
    }

    Value Value::eqZ() {
        return Value();
    }

    bool Value::eqZImpl() {
        return false;
    }

    Value Value::neqZ() {
        return Value();
    }

    bool Value::negZImpl() {
        return false;
    }

    bool Value::operator==(Value &r) {
        bool flag;
        if (type != r.type)
            return false;
        if (type == nullptr)
            return true;
        assert(tag() == r.tag());
        switch (tag()) {
            case array:
                if (array_.size() == r.array_.size()) {
                    for (size_t i = 0; i < array_.size(); ++i) {
                        if (array_[i] != r.array_[i]) {
                            return false;
                        }
                        return true;
                    }
                }
                return false;
            case qval:
                return qval_ == r.qval_;
            case intval:
                return intval_ == r.intval_;
            case uintval:
                return uintval_ == r.uintval_;
            case doubleval:
                return doubleval_ == r.doubleval_;
            case bval:
                return bval_ == r.bval_;
            case sval:
                return sval_ == r.sval_;
            case record:
                if (record_.size() != r.record_.size())
                    return false;
                for (auto &pair : record_) {
                    auto found = r.record_.find(pair.first);
                    if (found == r.record_.end() || pair.second != found->second) {
                        return false;
                    }
                }
                return true;
            default:
                return false;
        }
    }

    bool Value::operator>(Value &r) {
        return false;
    }

    bool Value::operator<(Value &r) {
        return false;
    }

    bool Value::operator!=(Value &r) {
        return !operator==(r);
    }

    bool Value::operator>=(Value &r) {
        return false;
    }

    bool Value::operator<=(Value &R) {
        return false;
    }

    Value Value::sqrt() {
        return Value();
    }

    Value Value::sin() {
        return Value();
    }

    Value Value::exp() {
        return Value();
    }

    Value Value::log() {
        return Value();
    }

    Value Value::asin() {
        return Value();
    }

    Value Value::cos() {
        return Value();
    }

    Value Value::acos() {
        return Value();
    }

    Value Value::tan() {
        return Value();
    }

    Value Value::atan() {
        return Value();
    }

    Value Value::classicalValue(Sigma state) {
        return Value();
    }

    bool Value::asBoolean() {
        return false;
    }

    bool Value::isZ() {
        return false;
    }

    size_t Value::asZ() {
        return 0;
    }

    bool Value::isQ() {
        return false;
    }

    std::string Value::toStringImpl(FormattingOptions opt) {
        return "";
    }

    std::string Value::toBasicStringImpl() {
        return std::string();
    }

    std::string Value::toString() {
        return std::string();
    }

    Value::~Value() {

    }

    QConst::QConst(std::shared_ptr<Value> constant): constant(constant) {}

    std::string QConst::toString() const {
        return constant->toStringImpl(FormattingOptions());
    }

    Value QConst::get(Sigma &sigma) {
        return *constant;
    }

    bool QConst::equals(QVal &other) {
        auto c = dynamic_cast<QConst*>(&other);
        if (!c)
            return false;
        return (*constant == *c->constant);
    }

    size_t QConst::toHash() const {
        return std::hash<Value>()(*constant) + 0x9e3779cd;
    }

    std::string QVar::toString() const {
        return "ref(" + std::to_string(ref_) + ")";
    }

    Value QVar::get(Sigma &sigma) {
        auto r = sigma.vars[ref_];
        if (consumedOnRead)
            removeVar(sigma);
        return r;
    }

    QVar QVar::dup(QState &state, Value self) {
        if (consumedOnRead) {
            consumedOnRead = false;
            return *this;
        }
        return QVal::dup(state, self);
    }

    std::shared_ptr<QVal> QVar::consumeOnRead() {
        consumedOnRead = true;
        return shared_from_this();
    }

    Value QVar::toVar(QState &state, Value self, bool cleanUp) {
        if (consumedOnRead) {
            consumedOnRead = false;
            if (cleanUp) {
                state.popFrameCleanup.push_back(shared_from_this());
            }
        }
        return self;
    }

    void QVar::assign(QState &state, Value rhs) {
        state.assignTo(ref_, rhs);
    }

    void QVar::removeVar(Sigma &sigma) {
        sigma.vars.erase(ref_);
    }

    bool QVar::equals(QVal &other) {
        if (auto c = dynamic_cast<QVar*>(&other)) {
            return ref_ == c->ref_ && consumedOnRead == c->consumedOnRead;
        }
        return false;
    }

    void QVar::forget(QState &state, Value rhs) {
        state.forget(ref_, rhs);
    }

    void QVar::forget(QState &state) {
        state.forget(ref_);
    }

    size_t QVar::toHash() const {
        size_t hash_value = 0x9e3779cf;
        hash_value ^= std::hash<size_t>()(ref_) + (hash_value << 6) + (hash_value >> 2);
        hash_value ^= std::hash<bool>()(consumedOnRead) + (hash_value << 6) + (hash_value >> 2);
        return hash_value;
    }

    ConvertQVal::ConvertQVal(std::shared_ptr<Value> value, std::shared_ptr<ast::Type> ntype)
        : value(value), ntype(ntype) {}

    Value ConvertQVal::get(Sigma &sigma) {
        return value->classicalValue(sigma).convertTo(*ntype);
    }

    void ConvertQVal::removeVar(Sigma &sigma) {
        value->removeVar(sigma);
    }

    bool ConvertQVal::equals(QVal &other) {
        if (auto c = dynamic_cast<ConvertQVal*>(&other)) {
            return (*value == *c->value && *ntype == *c->ntype);
        }
        return false;
    }

    void ConvertQVal::forget(QState &state, Value rhs) {
        value->forget(state, rhs);
    }

    void ConvertQVal::forget(QState &state) {
        value->forget(state);
    }

    size_t ConvertQVal::toHash() const {
        size_t hash_value = 0x9e3779d1;
        hash_value ^= std::hash<Value>()(*value) + (hash_value << 6) + (hash_value >> 2);
        hash_value ^= std::hash<ast::Type>()(*ntype) + (hash_value << 6) + (hash_value >> 2);
        return hash_value;
    }

    IndexQVal::IndexQVal(std::shared_ptr<Value> value, std::shared_ptr<Value> i)
        : value(value), i(i) {}

    Value IndexQVal::get(Sigma &sigma) {
        return value->classicalValue(sigma)[i->classicalValue(sigma)];
    }

    bool IndexQVal::equals(QVal &other) {
        if (auto c = dynamic_cast<IndexQVal*>(&other)) {
            return (*value == *c->value && *i == *c->i);
        }
        return false;
    }

    size_t IndexQVal::toHash() const {
        size_t hash_value = 0x9e3779d3;
        hash_value ^= std::hash<Value>()(*value) + (hash_value << 6) + (hash_value >> 2);
        hash_value ^= std::hash<Value>()(*i) + (hash_value << 6) + (hash_value >> 2);
        return hash_value;
    }

    Sigma::Ref Sigma::curRef = 0;

    Sigma Sigma::dup() const {
        return Sigma();
    }

    Sigma::Ref Sigma::assign(Sigma::Ref ref_, Value v) {
        return 0;
    }

    void Sigma::forget(Sigma::Ref ref_) {

    }

    void Sigma::forget(Sigma::Ref ref_, Value v) {

    }

    size_t Sigma::to_hash() const {
        size_t hash_value = 0x9e3779d5;
        for (auto &pair : vars) {
            hash_value ^= std::hash<size_t>()(pair.first) + (hash_value << 6) + (hash_value >> 2);
            hash_value ^= std::hash<Value>()(pair.second) + (hash_value << 6) + (hash_value >> 2);
        }
        return hash_value;
    }

    void Sigma::relabel(std::map<Ref, Ref> relabeling) {

    }

    Sigma::Sortable Sigma::toSortable() {
        return Sigma::Sortable();
    }

    std::string Sigma::toString() {
        return std::string();
    }

    Value makeTuple(ast::Type type, std::vector<Value> tuple) {
        return Value();
    }

    Value makeArray(ast::Type type, std::vector<Value> array) {
        return Value();
    }

    Value makeVector(ast::Type type, std::vector<Value> vector) {
        return Value();
    }

    Value makeRecord(ast::Type, Record record) {
        return Value();
    }

    Value makeQval(ast::Type &type, std::shared_ptr<QVal> qval) {
        return Value();
    }

    Value makeInteger(size_t value) {
        return Value();
    }

    Value makeInt(ast::Type type, BitInt<true> value) {
        return Value();
    }

    Value makeUint(ast::Type type, BitInt<false> value) {
        return Value();
    }

    Value makeBool(bool value) {
        return Value();
    }

    Value typeValue() {
        return Value();
    }

    Value pi() {
        return Value();
    }

    std::string formatQValue(QState qs, Value value) {
        return std::string();
    }

    long smallValue(size_t value) {
        return 0;
    }

    void HadamardUnitary::operator()(std::function<void(Value, std::complex<double>)> func, Value x) {

    }

    void XUnitary::operator()(std::function<void(Value, std::complex<double>)> func, Value x) {

    }

    void YUnitary::operator()(std::function<void(Value, std::complex<double>)> func, Value x) {

    }

    void ZUnitary::operator()(std::function<void(Value, std::complex<double>)> func, Value x) {

    }

    void RotX::operator()(std::function<void(Value, std::complex<double>)> func, Value x, double phi) {

    }

    void RotY::operator()(std::function<void(Value, std::complex<double>)> func, Value x, double phi) {

    }

    void RotZ::operator()(std::function<void(Value, std::complex<double>)> func, Value x, double phi) {

    }

    Value QState::dupValue(Value v) {
        return Value();
    }

    std::vector<Value> QState::dupValue(std::vector<Value> r) {
        return std::vector<Value>();
    }

    Record QState::dupValue(Record r) {
        return quint::Record();
    }

    std::string QState::toString() {
        return std::string();
    }

    void QState::dump() {

    }

    QState QState::dup() {
        return QState();
    }

    void QState::copyNonState(QState &rhs) {

    }

    void QState::add(Sigma k, std::complex<double> v) {

    }

    void QState::updateRelabeling(std::map<size_t, size_t> relabeling, Value to, Value from) {

    }

    void QState::operator+=(QState &r) {

    }

    Q<QState, QState> QState::split(Value cond) {
        return quint::Q<QState, QState>();
    }

    template<typename F, bool checkInterference, typename... T>
    QState QState::map(T... args) {
        return QState();
    }

    QState QState::unit() {
        return QState();
    }

    QState QState::pushFrame() {
        return QState();
    }

    QState QState::popFrame() {
        return QState();
    }

    void QState::passParameter(std::string prm, bool isConst, Value rhs) {

    }

    void QState::passContext() {

    }

    Value QState::call() {
        return Value();
    }

    Value QState::call(Value fun, Value arg, ast::Type type) {
        return Value();
    }

    QState QState::assertTrue(Value val) {
        return QState();
    }

    Value QState::readLocal(std::string s, bool constLookup) {
        return Value();
    }

    Value QState::makeFunction() {
        return Value();
    }

    void QState::declareFunction() {

    }

    Value QState::ite(Value cond, Value then, Value othw) {
        return Value();
    }

    Value QState::makeQVar(Value v) {
        return Value();
    }

    void QState::forget(size_t var, Value rhs) {

    }

    void QState::forget(size_t var) {

    }

    void QState::assignTo(size_t var, Value rhs) {

    }

    void QState::assignTo(Value &var, Value rhs) {

    }

    void QState::catAssignTo(Value &var, Value rhs) {

    }

    void QState::assignTo(std::string lhs, Value rhs) {

    }

    void QState::castAssignTo(std::string lhs, Value rhs) {

    }

    Value QState::H(Value x) {
        ast::BoolTy ty = ast::BoolTy(false);
        return x.applyUnitary<HadamardUnitary>(*this, ty);
    }

    Value QState::X(Value x) {
        return Value();
    }

    Value QState::Y(Value x) {
        return Value();
    }

    Value QState::Z(Value x) {
        return Value();
    }

    Value QState::phase(Value phi) {
        return Value();
    }

    Value QState::rX(Value args) {
        return Value();
    }

    Value QState::rY(Value args) {
        return Value();
    }

    Value QState::rZ(Value args) {
        return Value();
    }

    Value QState::array_(ast::Type type, Value arg) {
        return Value();
    }

    Value QState::reverse(ast::Type, Value arg) {
        return Value();
    }

    Value QState::measure(Value arg) {
        return Value();
    }

}
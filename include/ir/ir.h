//
// Created by BY210033 on 2023/3/14.
//

#ifndef QUINT_IR_H
#define QUINT_IR_H

#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <tuple>
#include <map>

#include "common/core.h"
#include "ir/snode.h"
#include "ir/type_factory.h"

#ifdef QUINT_WITH_LLVM
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/MapVector.h"
#endif

namespace quint::lang {

    class IRNode;
    class Block;
    class Stmt;
    class SNode;
    using pStmt = std::unique_ptr<Stmt>;

    enum class SNodeAccessFlag : int { block_local, read_only, mesh_local };

    class MemoryAccessOptions {
    private:
        std::unordered_map<SNode *, std::unordered_set<SNodeAccessFlag>> options_;

    public:
        void add_flag(SNode *snode, SNodeAccessFlag flag) {
            options_[snode].insert(flag);
        }

        bool has_flag(SNode *snode, SNodeAccessFlag flag) const {
            if (auto it = options_.find(snode); it != options_.end())
                return it->second.count(flag) != 0;
            else
                return false;
        }

        std::vector<SNode *> get_snode_with_flag(SNodeAccessFlag flag) const {
            std::vector<SNode *> snode;
            for (const auto &opt : options_) {
                if (has_flag(opt.first, flag)) {
                    snode.push_back(opt.first);
                }
            }
            return snode;
        }

        void clear() {
            options_.clear();
        }

        std::unordered_map<SNode *, std::unordered_set<SNodeAccessFlag>> get_all() const {
            return options_;
        }
    };

#ifdef QUINT_WITH_LLVM
    using stmt_vector = llvm::SmallVector<pStmt, 8>;
    using stmt_ref_vector = llvm::SmallVector<Stmt *, 2>;
#else
    using stmt_vector = std::vector<pStmt>;
    using stmt_ref_vector = std::vector<Stmt *>;
#endif

    class Identifier {
    public:
        std::string name_;
        int id{0};

        explicit Identifier(int id, const std::string &name = "")
            : name_(name), id(id) {
        }

        std::string raw_name() const;

        std::string name() const {
            return "@" + raw_name();
        }

        bool operator<(const Identifier &o) const {
            return id < o.id;
        }

        bool operator==(const Identifier &o) const {
            return id == o.id;
        }
    };

    class VecStatement {
    public:
        stmt_vector stmts;

        VecStatement() {
        }

        VecStatement(pStmt &&stmt) {
            push_back(std::move(stmt));
        }

        VecStatement(VecStatement &&other) {
            stmts = std::move(other.stmts);
        }

        VecStatement(stmt_vector &&other_stmts) {
            stmts = std::move(other_stmts);
        }

        Stmt *push_back(pStmt &&stmt);

        template<typename T, typename... Args>
        T *push_back(Args &&...args) {
            auto up = std::make_unique<T>(std::forward<Args>(args)...);
            auto ptr = up.get();
            stmts.push_back(std::move(up));
            return ptr;
        }

        pStmt  &back() {
            return stmts.back();
        }

        std::size_t size() const {
            return stmts.size();
        }

        pStmt &operator[](int i) {
            return stmts[i];
        }

    };

    class IRVisitor {
    public:
        bool allow_undefined_visitor;
        bool invoke_default_visitor;

        IRVisitor() {
            allow_undefined_visitor = false;
            invoke_default_visitor = false;
        }

        virtual ~IRVisitor() = default;

        // default visitor
        virtual void visit(Stmt *stmt) {
            if (!allow_undefined_visitor) {
                QUINT_ERROR(
                        "missing visitor function. Is the statement class registered via "
                        "DEFINE_VISIT?");
            }
        }

#define DEFINE_VISIT(T)            \
  virtual void visit(T *stmt) {    \
    if (allow_undefined_visitor) { \
      if (invoke_default_visitor)  \
        visit((Stmt *)stmt);       \
    } else                         \
      QUINT_NOT_IMPLEMENTED;          \
  }

        DEFINE_VISIT(Block);

#undef DEFINE_VISIT
    };

    class IRNode {
    public:
        virtual void accept(IRVisitor *visitor) {
            QUINT_NOT_IMPLEMENTED
        }

        virtual IRNode *get_parent() const = 0;

        IRNode *get_ir_root();

        virtual ~IRNode() = default;

        template <typename T>
        bool is() const {
            return dynamic_cast<const T*>(this) != nullptr;
        }

        template <typename T>
        T *as() {
            QUINT_ASSERT(is<T>())
            return dynamic_cast<T *>(this);
        }

        template<typename T>
        const T *as() const {
            QUINT_ASSERT(is<T>())
            return dynamic_cast<const T *>(this);
        }

        template<typename T>
        T *cast() {
            return dynamic_cast<T *>(this);
        }

        template<typename T>
        const T *cast() const {
            return dynamic_cast<const T *>(this);
        }

        std::unique_ptr<IRNode> clone();

    };

#define QUINT_DEFINE_ACCEPT \
  void accept(IRVisitor *visitor) override { visitor->visit(this); }

#define QUINT_DEFINE_CLONE                                             \
  std::unique_ptr<Stmt> clone() const override {                    \
    auto new_stmt =                                                 \
        std::make_unique<std::decay<decltype(*this)>::type>(*this); \
    new_stmt->mark_fields_registered();                             \
    new_stmt->io(new_stmt->field_manager);                          \
    return new_stmt;                                                \
  }

    class StmtField {
    public:
        StmtField() = default;

        virtual bool equal(const StmtField *other) const = 0;

        virtual ~StmtField() = default;
    };

    template<typename T>
    class StmtFieldNumeric final : public StmtField {
    private:
        std::variant<T *, T> value_;

    public:
        explicit StmtFieldNumeric(T *value) : value_(value) {
        }

        explicit StmtFieldNumeric(T value) : value_(value) {
        }

        bool equal(const StmtField *other_generic) const override {
            if (auto other = dynamic_cast<const StmtFieldNumeric *>(other_generic)) {
                if (std::holds_alternative<T *>(other->value_) &&
                    std::holds_alternative<T *>(value_)) {
                    return *(std::get<T *>(other->value_)) == *(std::get<T *>(value_));
                } else if (std::holds_alternative<T *>(other->value_) ||
                            std::holds_alternative<T *>(value_)) {
                    QUINT_ERROR("Inconsistent StmtFiled value types: a pointer value is compared "
                                "to a non-pointer value.")
                    return false;
                } else {
                    return std::get<T>(other->value_) == std::get<T>(value_);
                }
            } else {
                return false;
            }
        }
    };

    class StmtFieldManager {
    private:
        Stmt *stmt_;

    public:
        std::vector<std::unique_ptr<StmtField>> fields;

        explicit StmtFieldManager(Stmt *stmt) : stmt_(stmt) {
        }

        template<typename T>
        void operator()(const char *key, T &&value);

        template<typename T, typename... Args>
        void operator()(const char *key_, T &&t, Args **...rest) {
            std::string key(key_);
            size_t pos = key.find(',');
            std::string first_name = key.substr(0, pos);
            std::string rest_names =
                    key.substr(pos + 2, int(key.size()) - (int)pos - 2);
            this->operator()(first_name.c_str(), std::forward<T>(t));
            this->operator()(rest_names.c_str(), std::forward<Args>(rest)...);
        }

        bool equal(StmtFieldManager &other) const;
    };

#define QUINT_STMT_DEF_FIELDS(...)  \
    template <typename S>           \
    void io(S &serializer) const {  \
        QUINT_IO(__VA_ARGS__);      \
    }

#define QUINT_STMT_REG_FIELDS \
    mark_fields_registered(); \
    io(field_manager)

    class Stmt : public IRNode {
    protected:
        std::vector<Stmt **> operands;

    public:
        StmtFieldManager field_manager;
        static std::atomic<int> instance_id_counter;
        int instance_id;
        int id;
        Block *parent;
        bool erased;
        bool fields_registered;
        std::string tb;
        DataType ret_type;

        Stmt();
        Stmt(const Stmt &stmt);

        virtual bool is_container_statement() const {
            return false;
        }

        DataType &element_type() {
            return ret_type;
        }

        std::string ret_data_type_name() const {
            return ret_type.to_string();
        }

        std::string type_hint() const;

        std::string name() const {
            return fmt::format("${}", id);
        }

        std::string raw_name() const {
            return fmt::format("tmp{}", id);
        }

        QUINT_FORCE_INLINE int num_operands() const {
            return (int)operands.size();
        }

        QUINT_FORCE_INLINE Stmt *operand(int i) const {
            return *operands[i];
        }

        std::vector<Stmt *> get_operands() const;

        void set_operand(int i, Stmt *stmt);
        void register_operand(Stmt *&stmt);
        int locate_operand(Stmt **stmt);
        void mark_fields_registered();

        bool has_operand(Stmt *stmt) const;

        void replace_usages_with(Stmt *new_stmt);
        void replace_with(VecStatement &&new_statements, bool replace_usages = true);
        virtual void replace_operand_with(Stmt *old_stmt, Stmt *new_stmt);

        IRNode *get_parent() const override;

        Stmt *insert_before_me(std::unique_ptr<Stmt> &&new_stmt);

        Stmt *insert_after_me(std::unique_ptr<Stmt> &&new_stmt);

        virtual bool has_global_side_effect() const {
            return true;
        }

        virtual bool dead_instruction_eliminable() const {
            return !has_global_side_effect();
        }

        virtual bool common_statement_eliminable() const {
            return !has_global_side_effect();
        }

        template<typename T, typename... Args>
        static std::unique_ptr<T> make_typed(Args &&...args) {
            return std::make_unique<T>(std::forward<Args>(args)...);
        }

        void set_tb(const std::string &tb) {
            this->tb = tb;
        }

        std::string type();

        virtual std::unique_ptr<Stmt> clone() const {
            QUINT_NOT_IMPLEMENTED
        }

        ~Stmt() override = default;

        static void reset_counter() {
            instance_id_counter = 0;
        }

    };

    class Block : public IRNode {
    public:
        Stmt *parent_stmt{nullptr};
        stmt_vector statements;
        stmt_vector trash_bin;
        std::vector<SNode *> stop_gradients;

        // Only used in frontend. Stores LoopIndexStmt or BinaryOpStmt for loop
        // variables, and AllocaStmt for other variables.
        std::map<Identifier, Stmt *> local_var_to_stmt;

        Block() {
            parent_stmt = nullptr;
        }

        Block *parent_block() const;

        int locate(Stmt *stmt);
        stmt_vector::iterator locate(int location);
        stmt_vector::iterator find(Stmt *stmt);
        void erase(int location);
        void erase(Stmt *stmt);
        void erase_range(stmt_vector::iterator begin, stmt_vector::iterator end);
        void erase(std::unordered_set<Stmt *> stmts);
        std::unique_ptr<Stmt> extract(int location);
        std::unique_ptr<Stmt> extract(Stmt *stmt);

        // Returns stmt.get()
        Stmt *insert(std::unique_ptr<Stmt> &&stmt, int location = -1);
        Stmt *insert_at(std::unique_ptr<Stmt> &&stmt, stmt_vector::iterator location);

        // Returns stmt.back().get() or nullptr if stmt is empty
        Stmt *insert(VecStatement &&stmt, int location = -1);
        Stmt *insert_at(VecStatement &&stmt, stmt_vector::iterator location);

        void replace_statements_in_range(int start, int end, VecStatement &&stmts);
        void set_statements(VecStatement &&stmts);
        void replace_with(Stmt *old_statement,
                          std::unique_ptr<Stmt> &&new_statement,
                          bool replace_usages = true);
        void insert_before(Stmt *old_statement, VecStatement &&new_statements);
        void insert_after(Stmt *old_statement, VecStatement &&new_statements);
        void replace_with(Stmt *old_statement,
                          VecStatement &&new_statements,
                          bool replace_usages = true);
        Stmt *lookup_var(const Identifier &ident) const;

        IRNode *get_parent() const override;

        Stmt *back() const {
            return statements.back().get();
        }

        template<typename T, typename... Args>
        Stmt *push_back(Args &&...args) {
            auto stmt = std::make_unique<T>(std::forward<Args>(args)...);
            stmt->parent = this;
            statements.emplace_back(std::move(stmt));
            return back();
        }

        std::size_t size() const {
            return statements.size();
        }

        pStmt &operator[](int i) {
            return statements[i];
        }

        std::unique_ptr<Block> clone() const;

        QUINT_DEFINE_ACCEPT
    };

}

#endif //QUINT_IR_H

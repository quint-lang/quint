//
// Created by BY210033 on 2023/3/10.
//

#ifndef QUINT_FRONTEND_IR_H
#define QUINT_FRONTEND_IR_H

#include <string>
#include <vector>

#include "ir/ir.h"
#include "common/exceptions.h"
#include "ir/expression.h"
#include "ir/stmt_op_types.h"

namespace quint::lang {

    struct ForLoopConfig {
        bool is_bit_vectorized{false};
        int num_cpu_threads{0};
        bool strictly_serialized{false};
//        MemoryAccessOptions mem_access_opt;
        int block_dim{0};
        bool uniform{false};
    };


    class FrontendExternalFuncStmt : public Stmt {
    public:
        void *so_func;
        std::string asm_source;
        std::string bc_filename;
        std::string bc_func_name;
        std::vector<Expr> args;
        std::vector<Expr> outputs;

        FrontendExternalFuncStmt(void *so_func,
                                 const std::string &asm_source,
                                 const std::string &bc_filename,
                                 const std::string &bc_func_name,
                                 const std::vector<Expr> &args,
                                 const std::vector<Expr> &outputs)
            : so_func(so_func),
              asm_source(asm_source),
              bc_filename(bc_filename),
              bc_func_name(bc_func_name),
              args(args),
              outputs(outputs) {
        }

        QUINT_DEFINE_ACCEPT
    };

    class FrontendExprStmt : public Stmt {
    public:
        Expr val;

        explicit FrontendExprStmt(const Expr &val) : val(val) {
        }

        QUINT_DEFINE_ACCEPT
    };

    class FrontendAllocaStmt : public Stmt {
    public:
        Identifier ident;
        bool is_shared;

        FrontendAllocaStmt(const Identifier &lhs, DataType type)
            : ident(lhs), is_shared(false) {
            ret_type = type;
        }

        FrontendAllocaStmt(const Identifier &lhs,
                           std::vector<int> shape,
                           DataType element,
                           bool is_shared = false)
            : ident(lhs), is_shared(is_shared) {
        }

        QUINT_DEFINE_ACCEPT
    };

    class FrontendAssignStmt : public Stmt {
    public:
        Expr lhs, rhs;

        FrontendAssignStmt(const Expr &lhs, const Expr &rhs);

        QUINT_DEFINE_ACCEPT
    };

    class FrontendIfStmt : public Stmt {
    public:
        Expr condition;
        std::unique_ptr<Block> true_statements, false_statements;

        explicit FrontendIfStmt(const Expr &condition) : condition(condition) {
        }

        bool is_container_statement() const override {
            return true;
        }

        QUINT_DEFINE_ACCEPT
    };

    class FrontendPrintStmt : public Stmt {
    public:
        using EntryType = std::variant<Expr, std::string>;
        std::vector<EntryType> contents;

        explicit FrontendPrintStmt(const std::vector<EntryType> &contents_) {
            for (const auto &c : contents_) {
                if (std::holds_alternative<Expr>(c))
                    contents.push_back(std::get<Expr>(c));
                else
                    contents.push_back(c);
            }
        }

        QUINT_DEFINE_ACCEPT
    };

    class ArgLoadExpression : public Expression {
    public:
        int arg_id;
        DataType dt;
        bool is_ptr;

        ArgLoadExpression(int arg_id, DataType dt, bool is_ptr = false)
            : arg_id(arg_id), dt(dt), is_ptr(is_ptr) {
        }

        void type_check(CompileConfig *config) override;

        void flatten(FlattenContext *ctx) override;

        bool is_value() const override {
            return is_ptr;
        }

        QUINT_DEFINE_ACCEPT_FOR_EXPRESSION
    };

    class IdExpression : public Expression {
    public:
        Identifier id;

        explicit IdExpression(const Identifier &id): id(id) {}

        void type_check(CompileConfig *config) override {
        }

        void flatten(FlattenContext *ctx) override;

        bool is_value() const override {
            return true;
        }

        QUINT_DEFINE_ACCEPT_FOR_EXPRESSION
    };

    class ConstExpression : public Expression {
    public:
        TypedConstant val;

        template<typename T>
        explicit ConstExpression(const T &x): val(x) {
            ret_type = val.dt;
        }

        template<typename T>
        explicit ConstExpression(const DataType &dt, const T &x): val({dt, x}) {
            ret_type = val.dt;
        }

        void type_check(CompileConfig *config) override;

        void flatten(FlattenContext *ctx) override;

        QUINT_DEFINE_ACCEPT_FOR_EXPRESSION
    };

    class UnaryOpExpression : public Expression {
    public:
        UnaryOpType type;
        Expr operand;
        DataType cast_type;

        UnaryOpExpression(UnaryOpType type, const Expr &operand)
            : type(type), operand(operand) {
            cast_type = PrimitiveType::unknown;
        }

        UnaryOpExpression(UnaryOpType type, const Expr &operand, DataType cast_type)
            : type(type), operand(operand), cast_type(cast_type) {
        }

        bool is_cast() const;

        void type_check(CompileConfig *config) override;

        void flatten(FlattenContext *ctx) override;

        QUINT_DEFINE_ACCEPT_FOR_EXPRESSION
    };

    class BinaryOpExpression : public Expression {
    public:
        BinaryOpType type;
        Expr lhs, rhs;

        BinaryOpExpression(const BinaryOpType &type, const Expr &lhs, const Expr &rhs)
            : type(type), lhs(lhs), rhs(rhs) {
        }

        void type_check(CompileConfig *config) override;

        void flatten(FlattenContext *ctx) override;

        QUINT_DEFINE_ACCEPT_FOR_EXPRESSION
    };

    class TernaryOpExpression : public Expression {
    public:
        TernaryOpType type;
        Expr op1, op2, op3;

        TernaryOpExpression(TernaryOpType type,
                            const Expr &op1,
                            const Expr &op2,
                            const Expr &op3)
            : type(type) {
            this->op1.set(op1);
            this->op2.set(op2);
            this->op3.set(op3);
        }

        void type_check(CompileConfig *config) override;

        void flatten(FlattenContext *ctx) override;

        QUINT_DEFINE_ACCEPT_FOR_EXPRESSION
    };

    class ASTBuilder {
    private:
        enum LoopState { None, Outermost, Inner };
        enum LoopType { NotLoop, For, While };

        class ForLoopDecoratorRecorder {
        public:
            ForLoopConfig config;

            ForLoopDecoratorRecorder() {
                reset();
            }

            void reset() {
                config.is_bit_vectorized = false;
                config.num_cpu_threads = 0;
                config.uniform = false;
                config.block_dim = 0;
                config.strictly_serialized = false;
            }
        };

        std::vector<Block *> stack_;
        std::vector<LoopState> loop_state_stack_;
        ForLoopDecoratorRecorder for_loop_dec_;
        int id_counter_{0};
    public:
        ASTBuilder(Block *initial) {
            stack_.push_back(initial);
            loop_state_stack_.push_back(None);
        }

        void insert(std::unique_ptr<Stmt> &&stmt, int location = -1);

        Stmt *get_last_stmt();

        void insert_assignment(Expr &lhs,
                               const Expr &rhs,
                               const std::string &tb = "");

        Expr make_var(const Expr &x, std::string tb);

        void create_print(std::vector<std::variant<Expr, std::string>> contents);
        void begin_frontend_if(const Expr &cond);
        void begin_frontend_if_true();
        void begin_frontend_if_false();

        Expr expr_alloca();

        void create_scope(std::unique_ptr<Block> &list, LoopType tp = NotLoop);
        void pop_scope();

        Identifier get_next_id(const std::string &name = "") {
            return Identifier(id_counter_++, name);
        }
    };

    class FrontendContext {
    private:
        std::unique_ptr<ASTBuilder> current_builder_;
        std::unique_ptr<Block> root_node_;

    public:
        explicit FrontendContext() {
            root_node_ = std::make_unique<Block>();
            current_builder_ = std::make_unique<ASTBuilder>(root_node_.get());
        }

        ASTBuilder &builder() {
            return *current_builder_;
        }

        std::unique_ptr<Block> get_root() {
            return std::move(root_node_);
        }
    };




}

#endif //QUINT_FRONTEND_IR_H

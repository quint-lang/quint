//
// Created by BY210033 on 2023/3/30.
//

#ifndef QUINT_STATEMENTS_H
#define QUINT_STATEMENTS_H

#include "quint/ir/ir.h"
#include "quint/ir/mesh.h"
#include "quint/ir/offloaded_task_type.h"
#include "quint/ir/stmt_op_types.h"

#include <optional>

namespace quint::lang {

    class Function;

    class AllocaStmt : public Stmt {
    public:
        bool is_shared;

        explicit AllocaStmt(DataType type) : is_shared(false) {
            ret_type = type;
            QUINT_STMT_REG_FIELDS;
        }

        AllocaStmt(const std::vector<int> &shape,
                   DataType type,
                   bool is_shared = false) : is_shared(is_shared) {
//            ret_type = TypeFactory::crea
            QUINT_STMT_REG_FIELDS;
        }

        bool has_global_side_effect() const override {
            return false;
        }

        bool common_statement_eliminable() const override {
            return false;
        }

        QUINT_STMT_DEF_FIELDS(ret_type, is_shared);
        QUINT_DEFINE_CLONE
        QUINT_DEFINE_ACCEPT

    };

    class ArgLoadStmt : public Stmt {
    public:
        int arg_id;
        bool is_ptr;
        int field_dims_ = 0;

        ArgLoadStmt(int arg_id, const DataType &dt, bool is_ptr = false)
            : arg_id(arg_id) {
            this->ret_type = dt;
            this->is_ptr = is_ptr;
            this->field_dims_ = -1;
            QUINT_STMT_REG_FIELDS;
        }

        void set_extern_dims(int dims) {
            this->field_dims_ = dims;
        }

        bool has_global_side_effect() const override {
            return false;
        }

        QUINT_STMT_DEF_FIELDS(ret_type, arg_id, is_ptr);
        QUINT_DEFINE_ACCEPT
        QUINT_DEFINE_CLONE
    };

    class BinaryOpStmt : public Stmt {
    public:
        BinaryOpType op_type;
        Stmt *lhs, *rhs;
        bool is_bit_vectorized;

        BinaryOpStmt(BinaryOpType op_type, Stmt *lhs, Stmt *rhs, bool is_bit_vectorized = false)
            : op_type(op_type), lhs(lhs), rhs(rhs), is_bit_vectorized(is_bit_vectorized) {
            QUINT_ASSERT(!lhs->is<AllocaStmt>())
            QUINT_ASSERT(!rhs->is<AllocaStmt>())
            QUINT_STMT_REG_FIELDS;
        }

        bool has_global_side_effect() const override {
            return false;
        }

        QUINT_STMT_DEF_FIELDS(ret_type, op_type, lhs, rhs, is_bit_vectorized);
        QUINT_DEFINE_CLONE
        QUINT_DEFINE_ACCEPT
    };

    class AtomicOpStmt : public Stmt {
    public:
        AtomicOpType op_type;
        Stmt *dest;
        Stmt *val;
        bool is_reduction;

        AtomicOpStmt(AtomicOpType op_type, Stmt *dest, Stmt *val)
            : op_type(op_type), dest(dest), val(val), is_reduction(false) {
            QUINT_STMT_REG_FIELDS;
        }

        QUINT_STMT_DEF_FIELDS(ret_type, op_type, dest, val);
        QUINT_DEFINE_CLONE
        QUINT_DEFINE_ACCEPT
    };

    class ExternalPtrStmt : public Stmt {
    public:
        Stmt *base_ptr;
        std::vector<Stmt *> indices;
        std::vector<int> element_shape;
        int element_dim;

        ExternalPtrStmt(Stmt *basePtr, const std::vector<Stmt *> &indices);

        ExternalPtrStmt(Stmt *basePtr, const std::vector<Stmt *> &indices, const std::vector<int> &elementShape,
                        int elementDim);

        bool has_global_side_effect() const override {
            return false;
        }

        QUINT_STMT_DEF_FIELDS(ret_type, base_ptr, indices);
        QUINT_DEFINE_CLONE
        QUINT_DEFINE_ACCEPT

    };

    class GlobalPtrStmt : public Stmt {
    public:
        SNode *snode;
        std::vector<Stmt *> indices;
        bool activate;
        bool is_cell_access;
        bool is_bit_vectorized;

        GlobalPtrStmt(SNode *snode,
                      const std::vector<Stmt *> &indices,
                      bool activate = true,
                      bool is_cell_access = false);

        bool has_global_side_effect() const override {
            return activate;
        }

        bool common_statement_eliminable() const override {
            return true;
        }

        QUINT_STMT_DEF_FIELDS(ret_type, snode, indices, activate, is_bit_vectorized);
        QUINT_DEFINE_ACCEPT
        QUINT_DEFINE_CLONE
    };

    class OffloadedStmt : public Stmt {
    public:
        using TaskType = OffloadedTaskType;

        TaskType task_type;
        SNode *snode{nullptr};
        std::size_t begin_offset{0};
        std::size_t end_offset{0};
        bool const_begin{false};
        bool const_end{false};
        int32 begin_value{0};
        int32 end_value{0};
        int grid_dim{1};
        int block_dim{1};
        bool reversed{false};
        bool is_bit_vectorized{false};
        int num_cpu_threads{1};
        Stmt *end_stmt{nullptr};
        std::string range_hint = "";

        mesh::Mesh *mesh{nullptr};
        mesh::MeshElementType major_from_type;
        std::unordered_set<mesh::MeshElementType> major_to_types;
        std::unordered_set<mesh::MeshRelationType> minor_relation_types;

        std::unordered_map<mesh::MeshElementType, Stmt *> owned_offset_local;
        std::unordered_map<mesh::MeshElementType, Stmt *> total_offset_local;
        std::unordered_map<mesh::MeshElementType, Stmt *> owned_num_local;
        std::unordered_map<mesh::MeshElementType, Stmt *> total_num_local;

        std::vector<int> index_offsets;

        std::unique_ptr<Block> tls_prologue;
        std::unique_ptr<Block> mesh_prologue;
        std::unique_ptr<Block> bls_prologue;
        std::unique_ptr<Block> body;
        std::unique_ptr<Block> bls_epilogue;
        std::unique_ptr<Block> tls_epilogue;
        std::size_t tls_size{1};
        std::size_t bls_size{0};
        MemoryAccessOptions mem_access_opt;

        OffloadedStmt(TaskType task_type);

        std::string task_name() const;

        static std::string task_type_name(TaskType tt);

        bool has_body() const {
            return task_type != TaskType ::listgen && task_type != TaskType ::gc &&
                    task_type != TaskType ::gc_rc;
        }

        bool is_container_statement() const override {
            return has_body();
        }

        std::unique_ptr<Stmt> clone() const override;

        void all_blocks_accept(IRVisitor *visitor, bool skip_mesh_prologue = false);

        QUINT_STMT_DEF_FIELDS(ret_type /*inherited from Stmt*/,
                task_type,
                snode,
                begin_offset,
                end_offset,
                const_begin,
                const_end,
                begin_value,
                end_value,
                grid_dim,
                block_dim,
                reversed,
                num_cpu_threads,
                index_offsets,
                mem_access_opt);

        QUINT_DEFINE_ACCEPT

    };

    class LoopIndexStmt : public Stmt {
    public:
        Stmt *loop;
        int index;

        LoopIndexStmt(Stmt *loop, int index) : loop(loop), index(index) {
            QUINT_STMT_REG_FIELDS;
        }

        bool has_global_side_effect() const override {
            return false;
        }

        int max_num_bits() const;

        QUINT_STMT_DEF_FIELDS(ret_type, loop, index);
        QUINT_DEFINE_ACCEPT
        QUINT_DEFINE_CLONE
    };

    class ConstStmt : public Stmt {
    public:
        TypedConstant val;

        explicit ConstStmt(const TypedConstant &val) : val(val) {
            this->ret_type = val.dt;
            QUINT_STMT_REG_FIELDS;
        }

        bool has_global_side_effect() const override {
            return false;
        }

        QUINT_STMT_DEF_FIELDS(ret_type, val);
        QUINT_DEFINE_ACCEPT
        QUINT_DEFINE_CLONE
    };

    class RangeForStmt : public Stmt {
    public:
        Stmt *begin, *end;
        std::unique_ptr<Block> body;
        bool reversed;
        bool is_bit_vectorized;
        int num_cpu_threads;
        int block_dim;
        bool strictly_serialized;
        std::string range_hint;

        RangeForStmt(Stmt *begin,
                     Stmt *end,
                     std::unique_ptr<Block> &&body,
                     bool is_bit_vectorized,
                     int num_cpu_threads,
                     int block_dim,
                     bool strictly_serialized,
                     std::string range_hint = "");

        bool is_container_statement() const override {
            return true;
        }

        void reverse() {
            reversed = !reversed;
        }

        std::unique_ptr<Stmt> clone() const override;


        QUINT_STMT_DEF_FIELDS(
                begin,
                end,
                reversed,
                is_bit_vectorized,
                num_cpu_threads,
                block_dim,
                strictly_serialized);

        QUINT_DEFINE_ACCEPT
    };

    class StructForStmt : public Stmt {
    public:
        SNode *snode;
        std::unique_ptr<Block> body;
        std::unique_ptr<Block> block_initialization;
        std::unique_ptr<Block> block_finalization;
        std::vector<int> index_offsets;
        bool is_bit_vectorized;
        int num_cpu_threads;
        int block_dim;
        MemoryAccessOptions mem_access_opt;

        StructForStmt(SNode *snode,
                      std::unique_ptr<Block> &&body,
                      bool is_bit_vectorized,
                      int num_cpu_threads,
                      int block_dim);

        bool is_container_statement() const override {
            return true;
        }

        std::unique_ptr<Stmt> clone() const override;

        QUINT_STMT_DEF_FIELDS(snode,
                              index_offsets,
                              is_bit_vectorized,
                              num_cpu_threads,
                              block_dim,
                              mem_access_opt);
        QUINT_DEFINE_ACCEPT
    };

    class MeshForStmt : public Stmt {
    public:
        mesh::Mesh *mesh;
        std::unique_ptr<Block> body;
        bool is_bit_vectorized;
        int num_cpu_threads;
        int block_dim;
        mesh::MeshElementType major_from_type;
        std::unordered_set<mesh::MeshElementType> major_to_types{};
        std::unordered_set<mesh::MeshRelationType> minor_relation_types{};
        MemoryAccessOptions mem_access_opt;

        MeshForStmt(mesh::Mesh *mesh,
                    mesh::MeshElementType element_type,
                    std::unique_ptr<Block> &&body,
                    bool is_bit_vectorized,
                    int num_cpu_threads,
                    int block_dim);

        bool is_container_statement() const override {
            return true;
        }

        std::unique_ptr<Stmt> clone() const override;

        QUINT_STMT_DEF_FIELDS(mesh,
                              major_from_type,
                              major_to_types,
                              minor_relation_types,
                              is_bit_vectorized,
                              num_cpu_threads,
                              block_dim,
                              mem_access_opt);
        QUINT_DEFINE_ACCEPT

    };

    class FuncCallStmt : public Stmt {
    public:
        Function *func;
        std::vector<Stmt *> args;
        bool global_side_effect{true};

        FuncCallStmt(Function *func, const std::vector<Stmt *> &args);

        bool has_global_side_effect() const override {
            return global_side_effect;
        }

        QUINT_STMT_DEF_FIELDS(ret_type, func,args);
        QUINT_DEFINE_ACCEPT
        QUINT_DEFINE_CLONE

    };

    class ClearListStmt : public Stmt {
    public:
        explicit ClearListStmt(SNode *snode);

        SNode *snode;

        QUINT_STMT_DEF_FIELDS(ret_type, snode);
        QUINT_DEFINE_ACCEPT
        QUINT_DEFINE_CLONE
    };

    class GlobalLoadStmt : public Stmt {
    public:
        Stmt *src;

        explicit GlobalLoadStmt(Stmt *src) : src(src) {
            QUINT_STMT_REG_FIELDS;
        }

        bool has_global_side_effect() const override {
            return false;
        }

        bool common_statement_eliminable() const override {
            return false;
        }

        QUINT_STMT_DEF_FIELDS(ret_type, src);
        QUINT_DEFINE_ACCEPT
        QUINT_DEFINE_CLONE

    };

    class GlobalStoreStmt : public Stmt {
    public:
        Stmt *dest;
        Stmt *val;

        GlobalStoreStmt(Stmt *dest, Stmt *val) : dest(dest), val(val) {
            QUINT_STMT_REG_FIELDS;
        }

        bool common_statement_eliminable() const override {
            return false;
        }

        QUINT_STMT_DEF_FIELDS(ret_type, dest, val);
        QUINT_DEFINE_ACCEPT
        QUINT_DEFINE_CLONE
    };

    class IfStmt : public Stmt {
    public:
        Stmt *cond;
        std::unique_ptr<Block> true_statements, false_statements;

        explicit IfStmt(Stmt *cond);

        void set_true_statements(std::unique_ptr<Block> &&true_statements);
        void set_false_statements(std::unique_ptr<Block> &&false_statements);

        bool is_container_statement() const override {
            return true;
        }

        std::unique_ptr<Stmt> clone() const override;

        QUINT_STMT_DEF_FIELDS(cond);
        QUINT_DEFINE_ACCEPT
    };

    class GlobalTemporaryStmt : public Stmt {
    public:
        std::size_t offset;

        GlobalTemporaryStmt(std::size_t offset, const DataType &ret_type) : offset(offset) {
            this->ret_type = ret_type;
            QUINT_STMT_REG_FIELDS;
        }

        bool has_global_side_effect() const override {
            return false;
        }

        QUINT_STMT_DEF_FIELDS(ret_type, offset);
        QUINT_DEFINE_ACCEPT
        QUINT_DEFINE_CLONE
    };

    class LinearizeStmt : public Stmt {
    public:
        std::vector<Stmt *> inputs;
        std::vector<int> strides;

        LinearizeStmt(const std::vector<Stmt *> &inputs,
                      const std::vector<int> &strides) : inputs(inputs), strides(strides) {
            QUINT_ASSERT(inputs.size() == strides.size())
            QUINT_STMT_REG_FIELDS;
        }

        bool has_global_side_effect() const override {
            return false;
        }

        QUINT_STMT_DEF_FIELDS(ret_type, inputs, strides);
        QUINT_DEFINE_ACCEPT
        QUINT_DEFINE_CLONE
    };

    class BitExtractStmt : public Stmt {
    public:
        Stmt *input;
        int bit_begin, bit_end;
        bool simplified;
        BitExtractStmt(Stmt *input, int bit_begin, int bit_end)
            : input(input), bit_begin(bit_begin), bit_end(bit_end) {
            simplified = false;
            QUINT_STMT_REG_FIELDS;
        }

        bool has_global_side_effect() const override {
            return false;
        }

        QUINT_STMT_DEF_FIELDS(ret_type, input, bit_begin, bit_end, simplified);
        QUINT_DEFINE_ACCEPT
        QUINT_DEFINE_CLONE
    };

    class AdStackAllocaStmt : public Stmt {
    public:
        DataType dt;
        std::size_t max_size{0};

        AdStackAllocaStmt(const DataType &dt, std::size_t max_size) : dt(dt), max_size(max_size) {
            QUINT_STMT_REG_FIELDS;
        }

        bool has_global_side_effect() const override {
            return false;
        }

        bool common_statement_eliminable() const override {
            return false;
        }

        QUINT_STMT_DEF_FIELDS(ret_type, dt, max_size);
        QUINT_DEFINE_ACCEPT
        QUINT_DEFINE_CLONE
    };

}

#endif //QUINT_STATEMENTS_H

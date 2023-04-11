//
// Created by BY210033 on 2023/3/30.
//
#include "quint/ir/statements.h"

namespace quint::lang {

    OffloadedStmt::OffloadedStmt(OffloadedStmt::TaskType task_type)
        : task_type(task_type) {
        if (has_body()) {
            body = std::make_unique<Block>();
            body->parent_stmt = this;
        }
//        QUINT_STMT_REG_FIELDS;
    }

    std::string OffloadedStmt::task_name() const {
        if (task_type == TaskType::serial) {
            return "serial";
        } else if (task_type == TaskType::range_for) {
            return "range_for";
        } else if (task_type == TaskType::struct_for) {
            return "struct_for";
        } else if (task_type == TaskType::mesh_for) {
            return "mesh_for";
        } else if (task_type == TaskType::listgen) {
            QUINT_ASSERT(snode);
            return fmt::format("listgen_{}", snode->get_node_type_name_hinted());
        } else if (task_type == TaskType::gc) {
            QUINT_ASSERT(snode);
            return fmt::format("gc_{}", snode->name);
        } else if (task_type == TaskType::gc_rc) {
            return fmt::format("gc_rc");
        } else {
            QUINT_NOT_IMPLEMENTED
        }
    }

    std::string OffloadedStmt::task_type_name(OffloadedStmt::TaskType tt) {
        return offloaded_task_type_name(tt);
    }

    std::unique_ptr<Stmt> OffloadedStmt::clone() const {
        auto new_stmt = std::make_unique<OffloadedStmt>(task_type);
        new_stmt->snode = snode;
        new_stmt->begin_offset = begin_offset;
        new_stmt->end_offset = end_offset;
        new_stmt->const_begin = const_begin;
        new_stmt->const_end = const_end;
        new_stmt->begin_value = begin_value;
        new_stmt->end_value = end_value;
        new_stmt->grid_dim = grid_dim;
        new_stmt->block_dim = block_dim;
        new_stmt->reversed = reversed;
        new_stmt->is_bit_vectorized = is_bit_vectorized;
        new_stmt->num_cpu_threads = num_cpu_threads;
        new_stmt->index_offsets = index_offsets;

        new_stmt->mesh = mesh;
        new_stmt->major_from_type = major_from_type;
        new_stmt->major_to_types = major_to_types;
        new_stmt->minor_relation_types = minor_relation_types;

        new_stmt->owned_offset_local = owned_offset_local;
        new_stmt->total_offset_local = total_offset_local;
        new_stmt->owned_num_local = owned_num_local;
        new_stmt->total_num_local = total_num_local;

        if (tls_prologue) {
            new_stmt->tls_prologue = tls_prologue->clone();
            new_stmt->tls_prologue->parent_stmt = new_stmt.get();
        }
        if (mesh_prologue) {
            new_stmt->mesh_prologue = mesh_prologue->clone();
            new_stmt->mesh_prologue->parent_stmt = new_stmt.get();
        }
        if (bls_prologue) {
            new_stmt->bls_prologue = bls_prologue->clone();
            new_stmt->bls_prologue->parent_stmt = new_stmt.get();
        }
        if (body) {
            new_stmt->body = body->clone();
            new_stmt->body->parent_stmt = new_stmt.get();
        }
        if (bls_epilogue) {
            new_stmt->bls_epilogue = bls_epilogue->clone();
            new_stmt->bls_epilogue->parent_stmt = new_stmt.get();
        }
        if (tls_epilogue) {
            new_stmt->tls_epilogue = tls_epilogue->clone();
            new_stmt->tls_epilogue->parent_stmt = new_stmt.get();
        }
        new_stmt->tls_size = tls_size;
        new_stmt->bls_size = bls_size;
        new_stmt->mem_access_opt = mem_access_opt;
        return new_stmt;
    }

    void OffloadedStmt::all_blocks_accept(IRVisitor *visitor, bool skip_mesh_prologue) {
        if (tls_prologue)
            tls_prologue->accept(visitor);
        if (mesh_prologue && !skip_mesh_prologue)
            mesh_prologue->accept(visitor);
        if (bls_prologue)
            bls_prologue->accept(visitor);
        if (body)
            body->accept(visitor);
        if (bls_epilogue)
            bls_epilogue->accept(visitor);
        if (tls_epilogue)
            tls_epilogue->accept(visitor);
    }

    int LoopIndexStmt::max_num_bits() const {
        return 0;
    }

    ExternalPtrStmt::ExternalPtrStmt(Stmt *basePtr, const std::vector<Stmt *> &indices) : base_ptr(basePtr),
                                                                                          indices(indices) {
        QUINT_ASSERT(base_ptr != nullptr)
        QUINT_ASSERT(base_ptr->is<ArgLoadStmt>())
        QUINT_STMT_REG_FIELDS;
    }

    ExternalPtrStmt::ExternalPtrStmt(Stmt *basePtr, const std::vector<Stmt *> &indices,
                                     const std::vector<int> &elementShape, int elementDim)
        : ExternalPtrStmt(basePtr, indices) {
        this->element_shape = elementShape;
        this->element_dim = elementDim;
    }

    GlobalPtrStmt::GlobalPtrStmt(SNode *snode, const std::vector<Stmt *> &indices, bool activate, bool is_cell_access)
        : snode(snode), indices(indices), activate(activate), is_cell_access(is_cell_access), is_bit_vectorized(false) {
        QUINT_ASSERT(snode != nullptr)
        element_type() = snode->dt;
        QUINT_STMT_REG_FIELDS;
    }

    RangeForStmt::RangeForStmt(Stmt *begin, Stmt *end, std::unique_ptr<Block> &&body, bool is_bit_vectorized,
                               int num_cpu_threads, int block_dim, bool strictly_serialized, std::string range_hint)
                               : begin(begin), end(end), body(std::move(body)), is_bit_vectorized(is_bit_vectorized),
                                 num_cpu_threads(num_cpu_threads), block_dim(block_dim),
                                    strictly_serialized(strictly_serialized), range_hint(range_hint) {
        reversed = false;
        this->body->parent_stmt = this;
        QUINT_STMT_REG_FIELDS;
    }

    std::unique_ptr<Stmt> RangeForStmt::clone() const {
        auto new_stmt = std::make_unique<RangeForStmt>(
                begin, end, body->clone(), is_bit_vectorized, num_cpu_threads, block_dim, strictly_serialized);
        new_stmt->reversed = reversed;
        return new_stmt;
    }

    StructForStmt::StructForStmt(SNode *snode, std::unique_ptr<Block> &&body, bool is_bit_vectorized,
                                 int num_cpu_threads, int block_dim)
                                 : snode(snode), body(std::move(body)), is_bit_vectorized(is_bit_vectorized),
                                   num_cpu_threads(num_cpu_threads), block_dim(block_dim) {
        this->body->parent_stmt = this;
        QUINT_STMT_REG_FIELDS;
    }

    std::unique_ptr<Stmt> StructForStmt::clone() const {
        auto new_stmt = std::make_unique<StructForStmt>(snode, body->clone(),
                                                        is_bit_vectorized, num_cpu_threads, block_dim);
        return new_stmt;
    }

    MeshForStmt::MeshForStmt(mesh::Mesh *mesh, mesh::MeshElementType element_type, std::unique_ptr<Block> &&body,
                             bool is_bit_vectorized, int num_cpu_threads, int block_dim)
        : mesh(mesh),
          body(std::move(body)),
          is_bit_vectorized(is_bit_vectorized),
          num_cpu_threads(num_cpu_threads),
          block_dim(block_dim),
          major_from_type(element_type) {
        this->body->parent_stmt = this;
        QUINT_STMT_REG_FIELDS;
    }

    std::unique_ptr<Stmt> MeshForStmt::clone() const {
        auto new_stmt = std::make_unique<MeshForStmt>(
                mesh, major_from_type, body->clone(), is_bit_vectorized, num_cpu_threads, block_dim);
        new_stmt->major_to_types = major_to_types;
        new_stmt->minor_relation_types = minor_relation_types;
        new_stmt->mem_access_opt = mem_access_opt;
        return new_stmt;
    }

    FuncCallStmt::FuncCallStmt(Function *func, const std::vector<Stmt *> &args)
        : func(func), args(args) {
        QUINT_STMT_REG_FIELDS;
    }

    ClearListStmt::ClearListStmt(SNode *snode): snode(snode) {
        QUINT_STMT_REG_FIELDS;
    }

    IfStmt::IfStmt(Stmt *cond) {
        this->cond = cond;
        QUINT_STMT_REG_FIELDS;
    }

    void IfStmt::set_true_statements(std::unique_ptr<Block> &&new_true_statements) {
        true_statements = std::move(new_true_statements);
        if (true_statements)
            true_statements->parent_stmt = this;
    }

    void IfStmt::set_false_statements(std::unique_ptr<Block> &&new_false_statements) {
        false_statements = std::move(new_false_statements);
        if (false_statements)
            false_statements->parent_stmt = this;
    }

    std::unique_ptr<Stmt> IfStmt::clone() const {
        auto new_stmt = std::make_unique<IfStmt>(cond);
        if (true_statements) {
            new_stmt->set_true_statements(true_statements->clone());
        }
        if (false_statements) {
            new_stmt->set_false_statements(false_statements->clone());
        }
        return new_stmt;
    }
}
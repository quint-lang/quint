//
// Created by BY210033 on 2023/3/30.
//
#include "ir/statements.h"

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

}
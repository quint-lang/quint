//
// Created by BY210033 on 2023/3/30.
//

#ifndef QUINT_STATEMENTS_H
#define QUINT_STATEMENTS_H

#include "ir/ir.h"
#include "ir/mesh.h"
#include "ir/offloaded_task_type.h"

#include <optional>

namespace quint::lang {

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

}

#endif //QUINT_STATEMENTS_H

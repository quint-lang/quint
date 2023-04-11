//
// Created by BY210033 on 2023/3/15.
//

#ifndef QUINT_SNODE_H
#define QUINT_SNODE_H

#include "quint/ir/expr.h"
#include "quint/ir/type.h"
#include "quint/ir/snode_types.h"
#include "quint/inc/constant.h"
#include "quint/program/snode_expr_utils.h"

namespace quint::lang {

class Program;
class SNodeRWAccessorsBank;

    struct AxisExtractor {
        int num_elements_from_root{1};
        int shape{1};
        int acc_shape{1};
        int num_bits{0};
        int acc_offset{0};
        bool active{false};

        void activate(int num_bits) {
            active = true;
            this->num_bits = num_bits;
        }
    };

    class SNode {
    public:

        // This class decouples SNode from the frontend expression.
        class GradInfoProvider {

        };
        std::vector<std::unique_ptr<SNode>> ch;

        AxisExtractor extractors[quint_max_num_indices];
        std::vector<int> index_offsets;
        int num_active_indices{0};
        int physical_index_position[quint_max_num_indices]{};

        static std::atomic<int> counter;
        int id{0};
        int depth{0};

        std::string name;
        int64 num_cells_per_container{1};
        int total_num_bits{0};
        int total_bit_start{0};
        int chunk_size{0};
        std::size_t cell_size_bytes{0};
        std::size_t offset_bytes_in_parent_cell{0};
        DataType dt;
        bool has_ambient{false};
        TypedConstant ambient_val;
        SNode *parent{nullptr};

        // Quant
        PrimitiveType *physical_type{nullptr};
        int id_in_bit_struct{-1};
        bool is_bit_level{false};

        bool is_path_all_dense{false};

        std::string get_node_type_name_hinted() const;

        std::string node_type_name;
        SNodeType type;
        bool _morton{false};

        int64 max_num_elements() const {
            return num_cells_per_container;
        }

        explicit SNode(SNodeFieldMap *snode_to_fields = nullptr,
                       SNodeRWAccessorsBank *snode_rw_accessors_bank = nullptr);

        SNode(int depth,
              SNodeType type,
              SNodeFieldMap *snode_to_fields = nullptr,
              SNodeRWAccessorsBank *snode_rw_accessors_bank = nullptr);

        SNode(const SNode &);

        ~SNode() = default;

        std::string get_node_type_name() const;

    private:
        int snode_tree_id_{0};
        SNodeFieldMap *snode_to_fields_{nullptr};
        SNodeRWAccessorsBank *snode_rw_accessors_bank_{nullptr};
    };

}

#endif //QUINT_SNODE_H

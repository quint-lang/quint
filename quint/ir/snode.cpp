//
// Created by BY210033 on 2023/3/15.
//
#include "quint/ir/snode.h"
#include "quint/ir/statements.h"
#include "quint/program/snode_rw_accessors_bank.h"
#include "quint/program/program.h"

#include <limits>

namespace quint::lang {

    std::atomic<int> SNode::counter = 0;

    std::string SNode::get_node_type_name_hinted() const {
        return nullptr;
    }

    SNode::SNode(SNodeFieldMap *snode_to_fields, SNodeRWAccessorsBank *snode_rw_accessors_bank)
        : SNode(0, SNodeType::undefined, snode_to_fields, snode_rw_accessors_bank) {
    }

    SNode::SNode(int depth, SNodeType type, SNodeFieldMap *snode_to_fields,
                 SNodeRWAccessorsBank *snode_rw_accessors_bank)
        : depth(depth),
          type(type),
          snode_to_fields_(snode_to_fields),
          snode_rw_accessors_bank_(snode_rw_accessors_bank) {
        id = counter++;
        node_type_name = get_node_type_name();
        total_num_bits = 0;
        total_bit_start = 0;
        num_active_indices = 0;
        std::memset(physical_index_position, -1, sizeof(physical_index_position));
        parent = nullptr;
        has_ambient = false;
        dt = PrimitiveType::gen;
        _morton = false;
    }

    SNode::SNode(const SNode &) {
        QUINT_NOT_IMPLEMENTED
    }

    std::string SNode::get_node_type_name() const {
        return fmt::format("S{}", id);
    }

}
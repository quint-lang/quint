//
// Created by BY210033 on 2023/3/30.
//

#ifndef QUINT_MESH_H
#define QUINT_MESH_H

#include <atomic>
#include <map>
#include <unordered_set>

#include "quint/ir/type.h"
#include "quint/ir/snode.h"

namespace quint::lang {

    class Stmt;

    namespace mesh {

        enum class MeshTopology { Triangle = 3, Tetrahedron = 4 };

        enum class MeshElementType { Vertex = 0, Edge = 1, Face = 2, Cell = 3 };

        std::string element_type_name(MeshElementType type);

        enum class MeshRelationType {
            VV = 0,
            VE = 1,
            VF = 2,
            VC = 3,
            EV = 4,
            EE = 5,
            EF = 6,
            EC = 7,
            FV = 8,
            FE = 9,
            FF = 10,
            FC = 11,
            CV = 12,
            CE = 13,
            CF = 14,
            CC = 15,
        };

        enum class ConvType { l2g, l2r, g2r };

        struct MeshLocalRelation {
            MeshLocalRelation(SNode *value_, SNode *patch_offset_, SNode *offset_)
                : value(value_), patch_offset(patch_offset_), offset(offset_) {
                fixed = false;
            }

            explicit MeshLocalRelation(SNode *value_) : value(value_) {
                fixed = true;
            }

            bool fixed;
            SNode *value{nullptr};
            SNode *patch_offset{nullptr};
            SNode *offset{nullptr};
        };

        class Mesh {
        public:
            Mesh() = default;

            template<typename T>
            using MeshMapping = std::unordered_map<MeshElementType, T>;

            int num_patches{0};
            MeshMapping<int> num_elements{};
            MeshMapping<int> patch_max_element_num{};

            MeshMapping<SNode *> owned_offset{};
            MeshMapping<SNode *> total_offset{};
            std::map<std::pair<MeshElementType, ConvType>, SNode *> index_mapping{};

            std::map<MeshRelationType, MeshLocalRelation> relations;
        };

        struct MeshPtr {
            std::shared_ptr<Mesh> ptr;
        };

    }

}

#endif //QUINT_MESH_H

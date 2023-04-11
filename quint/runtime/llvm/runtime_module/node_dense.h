//
// Created by BY210033 on 2023/4/4.
//

#ifndef QUINT_NODE_DENSE_H
#define QUINT_NODE_DENSE_H

struct DenseMeta : public StructMeta {
    int morton_dim;
};

STRUCT_FIELD(DenseMeta, morton_dim)

i32 Dense_get_num_elements(Ptr meta, Ptr node) {
    return ((StructMeta *)meta)->max_num_elements;
}

void Dense_activate(Ptr meta, Ptr node, int i) {
    // Dense elements are always active
}

i32 Dense_is_active(Ptr meta, Ptr node, int i) {
    return 1;
}

Ptr Dense_lookup_element(Ptr meta, Ptr node, int i) {
    return node + ((StructMeta *)meta)->element_size * i;
}

#endif //QUINT_NODE_DENSE_H

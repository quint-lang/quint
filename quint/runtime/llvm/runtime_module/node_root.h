//
// Created by BY210033 on 2023/4/4.
//

#ifndef QUINT_NODE_ROOT_H
#define QUINT_NODE_ROOT_H

struct RootMeta : public StructMeta {
    int tag;
};

STRUCT_FIELD(RootMeta, tag);

void Root_activate(Ptr meta, Ptr node, int i) {
}

i32 Root_is_active(Ptr meta, Ptr node, int i) {
    return 1;
}

Ptr Root_lookup_element(Ptr meta, Ptr node, int i) {
    // only one element
    return node;
}

i32 Root_get_num_elements(Ptr meta, Ptr node) {
    return 1;
}


#endif //QUINT_NODE_ROOT_H

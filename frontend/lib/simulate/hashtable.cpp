//
// Created by BY210033 on 2023/5/24.
//
#include "simulate/hashtable.h"

namespace quint {

    template<typename K, typename V, bool (*eq)(K, V), size_t (*h)(K)>
    HashMap<K, V, eq, h>::HashMap() {

    }

    template<typename K, typename V, bool (*eq)(K, V), size_t (*h)(K)>
    void HashMap<K, V, eq, h>::realloc() {

    }

    template<typename K, typename V, bool (*eq)(K, V), size_t (*h)(K)>
    void HashMap<K, V, eq, h>::compact() {

    }

    template<typename K, typename V, bool (*eq)(K, V), size_t (*h)(K)>
    bool HashMap<K, V, eq, h>::insert(HashMap::E x) {
        auto hs = h(x.key);
        auto b = &es[hs];
        for (auto &it : b) {
            if (eq(x.key, it.key)) {
                it = x;
                return false;
            }
        }
        length++;
        b->push_back(x);
        if (b->size() > maxBucketSize && hs != h(b[0].key) && es.size() < 2 * length)
            realloc();
        return true;
    }

}
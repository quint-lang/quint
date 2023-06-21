//
// Created by BY210033 on 2023/5/18.
//

#ifndef QUINT_HASHTABLE_H
#define QUINT_HASHTABLE_H

#include <vector>
#include <string>
#include <cassert>

namespace quint {


    template<typename K, typename V, bool (*eq)(K, V), size_t (*h)(K)>
    class HashMap {
    public:
        static constexpr auto eq_ = eq;
        static constexpr auto h_ = h;
        struct E {
            K key;
            V value;
        };

        using B = std::vector<E>;
        int num_realloc;
        std::vector<B> es;
        size_t length;

        static constexpr size_t initialSize = 16;
        static constexpr size_t maxBucketSize = 20;
        static constexpr size_t limitFactor = 32;
        static constexpr size_t incrementFactor = 3;
        static constexpr size_t decrementFactor = 2;
        static constexpr size_t compactLimit = 16;

        HashMap();

        bool in(K key) {
            if (!es.empty()) {
                for (auto &e: es[h(key)]) {
                    if (eq(key, e.key)) {
                        return true;
                    }
                }
            }
            return false;
        }

        V get(K key, V alt) {
            if (!es.empty()) {
                for (auto &e: es[h(key)]) {
                    if (eq(key, e.key)) {
                        return e.value;
                    }
                }
            }
            return alt;
        }

        V &operator[](K k) {
            if (!es.empty()) {
                for (auto &e: es[h(k)]) {
                    if (eq(k, e.key)) {
                        return e.value;
                    }
                }
            }
            assert(false);
        }

        bool remove(K k) {
            if (!es.empty()) {
                auto &b = es[h(k)];
                for (auto it = b.begin(); it != b.end(); ++it) {
                    if (eq(k, it->key)) {
                        b.erase(it);
                        --length;
                        return true;
                    }
                }
            }
            return false;
        }


    private:
        void realloc();

        void compact();

        bool insert(E x);

    };

//template<typename T, typename eq, typename h>
//struct HSet {
//    HashMap<T, void *, eq, h> map;
//};

} // namespace quint
#endif //QUINT_HASHTABLE_H

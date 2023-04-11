//
// Created by BY210033 on 2023/2/7.
//

#ifndef QUINT_RANDOM_ENGINE_H
#define QUINT_RANDOM_ENGINE_H

#include <random>
#include <ctime>

namespace quint {
    using seed_t = decltype(std::time(0));
    using engine_t = std::mt19937_64;
    struct random_engine
    {
        seed_t seed = 10101;
        std::mt19937_64 reng;

        random_engine() {}

        inline static random_engine& get_instance()
        {
            static random_engine inst;
            return inst;
        }

        inline engine_t& _get_engine()
        {
            return reng;
        }

        inline static engine_t& get_engine()
        {
            return get_instance()._get_engine();
        }

        inline double _rng()
        {
            static std::uniform_real_distribution<double> ud(0, 1);
            return ud(reng);
        }

        inline static double rng()
        {
            return get_instance()._rng();
        }

        inline double _uniform01()
        {
            return _rng();
        }

        inline static double uniform01()
        {
            return get_instance()._uniform01();
        }

        inline void set_seed(seed_t _seed)
        {
            seed = _seed;
            reng.seed(_seed);
        }
        inline unsigned int reseed()
        {
            set_seed(seed_t(_rng() * std::numeric_limits<seed_t>::max()));
            return seed;
        }
        inline seed_t get_seed() { return seed; }
        inline static seed_t time_seed()
        {
            seed_t seed = time(0);
            get_instance().set_seed(seed);
            return seed;
        }
    };

    template<typename Ty>
    std::string get_random_hash_str(const Ty& value)
    {
        size_t hashed = std::hash<Ty>()(value) ^
                        std::hash<double>()(random_engine::get_instance().uniform01());

        char buf[64];
        std::sprintf(buf, "%.16zX", hashed);
        return buf;
    }

    inline std::string get_random_hash_str()
    {
        size_t hashed = std::hash<double>()(random_engine::get_instance().uniform01());

        char buf[64];
        std::sprintf(buf, "%.16zX", hashed);
        return buf;
    }

}// namespace quint

#endif //QUINT_RANDOM_ENGINE_H

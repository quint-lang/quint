//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_DICT_H
#define QUINT_DICT_H

#include <map>
#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <typeinfo>

#include "common/core.h"

namespace quint {

    class Dict {
    private:
        std::map<std::string, std::string> data_;

    public:
//        QUINT_IO_DEF(data_);

        Dict() = default;

        template <typename T>
        Dict(const std::string &key, const T &value) {
            this->set(key, value);
        }

        std::vector<std::string> get_keys() const {
            std::vector<std::string> keys;
            for (auto it = data_.begin(); it != data_.end(); ++it) {
                keys.push_back(it->first);
            }
            return keys;
        }

        void clear() {
            data_.clear();
        }

//        template <typename V>
//        typename std::enable_if_t<(!type::is_VectorND<V>() &&
//                                   !std::is_reference<V>::value &&
//                                   !std::is_pointer<V>::value),
//                V>
//        get(std::string key) const;

        static bool is_string_integral(const std::string &str) {
            // TODO: make it correct
            if (str.find('.') != std::string::npos) {
                return false;
            }
            if (str.find('e') != std::string::npos) {
                return false;
            }
            if (str.find('E') != std::string::npos) {
                return false;
            }
            return true;
        }

        void check_string_integral(const std::string &str) const {
            if (!is_string_integral(str)) {
                QUINT_ERROR(
                        "Getting integral value out of non-integral string '{}' is not "
                        "allowed.",
                        str);
            }
        }

        void check_value_integral(const std::string &key) const {
            auto str = get_string(key);
            check_string_integral(str);
        }

//        template <
//                typename V,
//                typename std::enable_if<(type::is_VectorND<V>()), V>::type * = nullptr>
//        V get(std::string key) const {
//            constexpr int N = V::dim;
//            using T = typename V::ScalarType;
//
//            std::string str = this->get_string(key);
//            std::string temp;
//            if (str[0] == '(') {
//                temp = "(";
//            } else if (str[0] == '[') {
//                temp = "[";
//            }
//            if (std::is_integral<T>()) {
//                check_string_integral(str);
//            }
//            for (int i = 0; i < N; i++) {
//                std::string placeholder;
//                if (std::is_same<T, float32>()) {
//                    placeholder = "%f";
//                } else if (std::is_same<T, float64>()) {
//                    placeholder = "%lf";
//                } else if (std::is_same<T, int32>()) {
//                    placeholder = "%d";
//                } else if (std::is_same<T, uint32>()) {
//                    placeholder = "%u";
//                } else if (std::is_same<T, int64>()) {
//#ifdef WIN32
//                    placeholder = "%I64d";
//#else
//                    placeholder = "%lld";
//#endif
//                } else if (std::is_same<T, uint64>()) {
//#ifdef WIN32
//                    placeholder = "%I64u";
//#else
//                    placeholder = "%llu";
//#endif
//                } else {
//                    assert(false);
//                }
//                temp += placeholder;
//                if (i != N - 1) {
//                    temp += ",";
//                }
//            }
//            if (str[0] == '(') {
//                temp += ")";
//            } else if (str[0] == '[') {
//                temp += "]";
//            }
//            VectorND<N, T> ret;
//            if (N == 1) {
//                sscanf(str.c_str(), temp.c_str(), &ret[0]);
//            } else if (N == 2) {
//                sscanf(str.c_str(), temp.c_str(), &ret[0], &ret[1]);
//            } else if (N == 3) {
//                sscanf(str.c_str(), temp.c_str(), &ret[0], &ret[1], &ret[2]);
//            } else if (N == 4) {
//                sscanf(str.c_str(), temp.c_str(), &ret[0], &ret[1], &ret[2], &ret[3]);
//            }
//            return ret;
//        }

        std::string get(std::string key, const char *default_val) const;

        template <typename T>
        T get(std::string key, const T &default_val) const;

        bool has_key(std::string key) const {
            return data_.find(key) != data_.end();
        }

//        std::vector<std::string> get_string_arr(std::string key) const {
//            std::string str = get_string(key);
//            std::vector<std::string> strs = split_string(str, ",");
//            for (auto &s : strs) {
//                s = trim_string(s);
//            }
//            return strs;
//        }

        template <typename T>
        T *get_ptr(std::string key) const {
            std::string val = get_string(key);
            std::stringstream ss(val);
            std::string t;
            int64 ptr_ll;
            std::getline(ss, t, '\t');
            ss >> ptr_ll;
            QUINT_ASSERT_INFO(t == typeid(T).name(),
                           "Pointer type mismatch: " + t + " and " + typeid(T).name());
            return reinterpret_cast<T *>(ptr_ll);
        }

        template <typename T>
        std::enable_if_t<std::is_pointer<T>::value, std::remove_pointer_t<T>> get(
                std::string key) const {
            return get_ptr<std::remove_pointer_t<T>>(key);
        }

        template <typename T>
        std::enable_if_t<std::is_reference<T>::value, std::remove_reference_t<T>>
        &get(std::string key) const {
            return *get_ptr<std::remove_reference_t<T>>(key);
        }

        template <typename T>
        T *get_ptr(std::string key, T *default_value) const {
            if (has_key(key)) {
                return get_ptr<T>(key);
            } else {
                return default_value;
            }
        }

        template <typename T>
        Dict &set(std::string name, T val) {
            std::stringstream ss;
            ss << val;
            data_[name] = ss.str();
            return *this;
        }

        Dict &set(std::string name, const char *val) {
            std::stringstream ss;
            ss << val;
            data_[name] = ss.str();
            return *this;
        }

//        Dict &set(std::string name, const Vector2 &val) {
//            std::stringstream ss;
//            ss << "(" << val.x << "," << val.y << ")";
//            data_[name] = ss.str();
//            return *this;
//        }
//
//        Dict &set(std::string name, const Vector3 &val) {
//            std::stringstream ss;
//            ss << "(" << val.x << "," << val.y << "," << val.z << ")";
//            data_[name] = ss.str();
//            return *this;
//        }
//
//        Dict &set(std::string name, const Vector4 &val) {
//            std::stringstream ss;
//            ss << "(" << val.x << "," << val.y << "," << val.z << "," << val.w << ")";
//            data_[name] = ss.str();
//            return *this;
//        }
//
//        Dict &set(std::string name, const Vector2i &val) {
//            std::stringstream ss;
//            ss << "(" << val.x << "," << val.y << ")";
//            data_[name] = ss.str();
//            return *this;
//        }
//
//        Dict &set(std::string name, const Vector3i &val) {
//            std::stringstream ss;
//            ss << "(" << val.x << "," << val.y << "," << val.z << ")";
//            data_[name] = ss.str();
//            return *this;
//        }
//
//        Dict &set(std::string name, const Vector4i &val) {
//            std::stringstream ss;
//            ss << "(" << val.x << "," << val.y << "," << val.z << "," << val.w << ")";
//            data_[name] = ss.str();
//            return *this;
//        }

        template <typename T>
        static std::string get_ptr_string(T *ptr) {
            std::stringstream ss;
            ss << typeid(T).name() << "\t" << reinterpret_cast<uint64>(ptr);
            return ss.str();
        }

        template <typename T>
        Dict &set(std::string name, T *const ptr) {
            data_[name] = get_ptr_string(ptr);
            return *this;
        }

        std::string get_string(std::string key) const {
            if (data_.find(key) == data_.end()) {
                QUINT_ERROR("No key named '{}' found.", key);
            }
            return data_.find(key)->second;
        }

        template <typename T>
        Dict &operator()(const std::string &key, const T &value) {
            this->set(key, value);
            return *this;
        }
    };

    using Config = Dict;

}

#endif //QUINT_DICT_H

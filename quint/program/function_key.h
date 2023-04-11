//
// Created by BY210033 on 2023/4/11.
//

#ifndef QUINT_FUNCTION_KEY_H
#define QUINT_FUNCTION_KEY_H

#include <functional>
#include <string>

namespace quint::lang {

    class FunctionKey {
    public:
        std::string func_name;
        int func_id;
        int instance_id;

        FunctionKey(const std::string &func_name, int func_id, int instance_id);

        bool operator==(const FunctionKey &other_key) const;

        [[nodiscard]] std::string get_full_name() const;
    };

}

namespace std {
    template <>
    struct hash<quint::lang::FunctionKey> {
        std::size_t operator()(const quint::lang::FunctionKey &key) const noexcept {
            return key.func_id ^ (key.instance_id << 16);
        }
    };
}  // namespace std

#endif //QUINT_FUNCTION_KEY_H

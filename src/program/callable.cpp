//
// Created by BY210033 on 2023/3/10.
//
#include "program/callable.h"
#include "program/program.h"

namespace quint::lang {

    Callable::Callable() = default;

    Callable::~Callable() = default;

    int Callable::insert_scalar_arg(const DataType &dt) {
        args.emplace_back(dt.get_element_type(), false);
        return (int) args.size() - 1;
    }

    int Callable::insert_arr_arg(const DataType &dt, int total_dim, std::vector<int> element_shape) {
        args.emplace_back(dt->get_compute_type(), true, 0, total_dim, element_shape);
        return (int) args.size() - 1;
    }

    int Callable::insert_texture_arg(const DataType &dt) {
        args.emplace_back(dt->get_compute_type(), true);
        return (int)args.size() - 1;
    }

    int Callable::insert_ret(const DataType &dt) {
        rets.emplace_back(dt.get_element_type());
        return (int) rets.size() - 1;
    }

    void Callable::finalize_rets() {
        std::vector<const Type *> types;
        types.reserve(rets.size());
        for (const auto &ret : rets) {
            types.push_back(ret.dt);
        }
        ret_type = TypeFactory::get_instance().get_struct_type(types)->as<StructType>();
    }


}
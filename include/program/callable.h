//
// Created by BY210033 on 2023/3/10.
//

#ifndef QUINT_CALLABLE_H
#define QUINT_CALLABLE_H

#include "ir/type.h"
#include "ir/type_utils.h"
#include "ir/type_factory.h"

#include <memory>

namespace quint::lang {

    class Program;
    class IRNode;
    class FrontendContext;

    class Callable {
    public:
        Program *program{nullptr};
        std::unique_ptr<IRNode> ir{nullptr};
        std::unique_ptr<FrontendContext> context{nullptr};

        struct Arg {
            bool is_array {false};
            std::size_t total_dim{0};

            explicit Arg(const DataType &dt = PrimitiveType::unknown,
                         bool is_array = false,
                         std::size_t size_unused = 0,
                         int total_dim = 0,
                         std::vector<int> element_shape = {}) {
                // todo handler tensor type
                if (dt->is<PrimitiveType>() && element_shape.size() > 0) {
//                    this->dt_ = TypeFactory::get_instance().
                } else {
                    this->dt_ = dt;
                }

                this->is_array = is_array;
                this->total_dim = total_dim;
            }

            std::vector<int> get_element_shape() const {
                return dt_.get_shape();
            }

            DataType get_element_type() const {
                return dt_.get_element_type();
            }

            int get_element_size() const {
                return data_type_size(dt_);
            }

            DataType get_dtype() const {
                return dt_;
            }

        private:
            DataType dt_;
        };

        struct Ret {
            DataType dt;

            explicit Ret(const DataType &dt = PrimitiveType::unknown): dt(dt) {}
        };

        std::vector<Arg> args;
        std::vector<Ret> rets;

        const StructType *ret_type = nullptr;

        Callable();
        virtual ~Callable();

        int insert_scalar_arg(const DataType &dt);

        int insert_arr_arg(const DataType &dt,
                           int total_dim,
                           std::vector<int> element_shape);

        int insert_texture_arg(const DataType &dt);

        int insert_ret(const DataType &dt);

        void finalize_rets();

        [[nodiscard]] virtual std::string get_name() const = 0;

        class CurrentCallableGuard {
            Callable *old_callable_;
            Program *program_;

        public:
            CurrentCallableGuard(Program *program, Callable *callable);

            ~CurrentCallableGuard();
        };
    };

}

#endif //QUINT_CALLABLE_H

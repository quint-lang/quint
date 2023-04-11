//
// Created by BY210033 on 2023/2/7.
//

#ifndef QUINT_DATA_STRUCTURE_H
#define QUINT_DATA_STRUCTURE_H

#include "quint/common/logger.h"
#include "quint/util/misc.h"

#include <stack>

namespace quint {

    struct DAG;

#define ConditionSatisfied(state_ref) \
	std::all_of(std::begin(condition_variable), std::end(condition_variable),\
	[&](int cond_var)\
	{\
		return state_ref.get(cond_var).as_bool();\
	})\

#define ClassControllable \
    inline auto controlled_by(Register& cond)\
    {\
        control_bit.push_back(std::make_shared<Register>(cond));\
        return *this;\
    }                     \

#define ControlAll \
    inline auto controlled_by(std::vector<std::shared_ptr<Register>> conds) \
    {              \
        for (auto cond : conds) control_bit.push_back(cond);           \
    }              \


    inline const std::string& get_name(const StateInfoType& m)
    {
        return std::get<0>(m);
    }

    inline const StateStorageType& get_type(const StateInfoType& m)
    {
        return std::get<1>(m);
    }

    inline const size_t& get_size(const StateInfoType& m)
    {
        return std::get<2>(m);
    }

    inline bool get_status(const StateInfoType& m)
    {
        return std::get<3>(m);
    }

    struct StateStorage
    {
        inline static const char* get_type_str(StateStorageType type) {
            static const char* typestr[] = {
                    "Reg",
                    "UInt",
                    "Int",
                    "Bool",
                    "Rat"
            };
            return typestr[type];
        }

        size_t value = 0;

        template<typename Ty>
        Ty as(size_t size) const
        {
            if constexpr (std::is_floating_point_v<Ty>)
            {
                return 1.0 * value / pow2(size);
            }
            else if constexpr (std::is_integral_v<Ty>)
            {
                if constexpr (std::is_same_v<Ty, bool>)
                {
                    return bool(value);
                }
                else if constexpr (std::is_signed_v<Ty>)
                {
                    return get_complement(value, size);
                }
                else
                {
                    if (size == 64) return value;
                    return value % pow2(size);
                }
            }
            else {
                throw_invalid_input();

                // static_assert(false, "StateStorage::as<Ty> unsupported type.");
            }
        }

        bool as_bool() const
        {
            return bool(value & 1);
        }

        StateStorage()
        {}

        inline bool operator==(const StateStorage& rhs) const
        {
            return value == rhs.value;
        }

        inline bool operator<(const StateStorage& rhs) const
        {
            return value < rhs.value;
        }

        inline bool operator>(const StateStorage& rhs) const
        {
            return value > rhs.value;
        }

        inline std::string to_string(const StateInfoType& info) const
        {
            if (!get_status(info))
                return std::string();

            switch (get_type(info))
            {
                case General:
                    return fmt::format("|{}>", dec2bin(value, get_size(info)));
                case SignedInteger:
                    return fmt::format("|{}>", get_complement(value, get_size(info)));
                case UnsignedInteger:
                    return fmt::format("|{}>", as<uint64_t>(get_size(info)));
                case Boolean:
                    return fmt::format("|{}>", as<bool>(get_size(info)));
                case Rational:
                    return fmt::format("|{}>", as<double>(get_size(info)));
                default:
                    throw_bad_switch_case();
                    return "";
            }
        }

        inline std::string to_binary_string(const StateInfoType& info) const
        {
            return fmt::format("|{}>", dec2bin(value, get_size(info)));
        }
        inline void flip(size_t digit)
        {
            value = flip_digit(value, digit);
        }
    };

    struct System
    {
        constexpr static size_t CachedRegisterSize = 64;
        complex_t amplitude = 1.0;
        // std::vector<StateStorage> registers;

        std::array<StateStorage, CachedRegisterSize> registers;
        static std::vector<StateInfoType> name_register_map;
        static size_t max_qubit_count;
        static size_t max_register_count;
        static size_t max_system_size;
        static std::vector<int> temporal_registers;
        static std::vector<int> reusable_registers;

        inline auto& get(int id)
        {
            return registers[id];
        }

        inline auto& get(int id) const
        {
            return registers[id];
        }

        inline static void clear()
        {
            name_register_map.clear();
            temporal_registers.clear();
            reusable_registers.clear();
            max_qubit_count = 0;
            max_register_count = 0;
            max_system_size = 0;

            // temporal_register_name.clear();
        }

        inline static size_t get_qubit_count()
        {
            size_t count = 0;
            for (const auto& m : name_register_map)
            {
                if (std::get<3>(m))
                    count += std::get<2>(m);
            }
            return count;
        }

        inline static size_t get_activated_register_size()
        {
            //return name_register_map.size() - reusable_registers.size();
            size_t count = 0;
            for (const auto& m : name_register_map)
            {
                if (std::get<3>(m))
                    count += 1;
            }
            return count;
        }

        inline StateStorage& last_register()
        {
            if (name_register_map.size() == 0)
                throw_general_runtime_error();

            return registers[get_activated_register_size() - 1];
        }

        inline const StateStorage& last_register() const
        {
            if (name_register_map.size() == 0)
                throw_general_runtime_error();

            return registers[get_activated_register_size() - 1];
        }

        inline static void update_max_size(size_t new_size)
        {
            if (new_size > max_system_size)
                max_system_size = new_size;
        }

        inline static int get(const std::string& name)
        {
#ifdef SINGLE_THREAD
            profiler _("System::get");
#endif
            for (int i = name_register_map.size() - 1; i >= 0; --i)
            {
                if (!status_of(i))
                    continue;
                if (name == std::get<0>(name_register_map[i]))
                    return i;
            }
            return -1;
        }
        inline static StateInfoType get_register_info(std::string name)
        {
            auto iter = std::find_if(name_register_map.begin(),
                                     name_register_map.end(),
                                     [name](const StateInfoType& n)
                                     {
                                         return std::get<0>(n) == name;
                                     }
            );

            return *iter;
        }

        inline static std::string name_of(int id)
        {
            const auto& info = name_register_map[id];
            return std::get<0>(info);
        }

        inline static size_t size_of(std::string name)
        {
            profiler _("System::size_of");
            auto&& info = get_register_info(name);
            return std::get<2>(info);
        }

        inline static size_t size_of(int id)
        {
            const auto& info = name_register_map[id];
            return std::get<2>(info);
        }

        inline static StateStorageType type_of(std::string name)
        {
            profiler _("System::type_of");
            auto&& info = get_register_info(name);
            return std::get<1>(info);
        }

        inline static StateStorageType type_of(int id)
        {
            const auto& info = name_register_map[id];
            return std::get<1>(info);
        }

        inline static bool status_of(int id)
        {
            return get_status(name_register_map[id]);
        }

        inline static int add_register(
                std::string name, StateStorageType type, size_t size)
        {
            /* DEBUG */
            if (name_register_map.size() >= CachedRegisterSize)
            {
                // try to reuse the space
                if (reusable_registers.size() == 0)
                    // all spaces are run out
                    throw_general_runtime_error();
                else {
                    int reg_id = reusable_registers.back();
                    // fmt::print("Add    {}: {}\n", reg_id, name_register_map[reg_id]);
                    name_register_map[reg_id] = { name, type, size, true };

                    reusable_registers.pop_back();
                    return reg_id;
                }
            }
                // if (false) {}

            else {
                name_register_map.emplace_back(name, type, size, true);
                if (get_qubit_count() > max_qubit_count)
                    max_qubit_count = get_qubit_count();
                if (get_activated_register_size() > max_register_count)
                    max_register_count = get_activated_register_size();

                return name_register_map.size() - 1;
            }

            return -1;
        }

        inline static int add_register_synchronous(
                std::string name, StateStorageType type, size_t size)
        {
            int ret = add_register(name, type, size);

            /* FOR DEBUG */
            //for (auto& s : system_states)
            //{
            //	if (s.GetAs(ret, uint64_t) != 0)
            //	{
            //		throw_general_runtime_error();
            //	}
            //}
            return ret;
        }

        inline static void remove_register(int id)
        {
            if (id < 0)
                throw_invalid_input();

            std::get<3>(name_register_map[id]) = false;
            reusable_registers.push_back(id);
            // fmt::print("Remove {}: {}\n", id, name_register_map[id]);
            /* FOR DEBUG */
            /*name_register_map[id] = name_register_map.back();
            name_register_map.pop_back();*/

        }

        inline static void remove_register(std::string name)
        {
            int id = get(name);
            remove_register(id);
        }

        inline static void remove_register_synchronous(int id,
                                                       std::vector<System>& state)
        {
#ifdef SINGLE_THREAD
            for (auto& s : state)
			{
#else
#pragma omp parallel for
            for (int i = 0; i < state.size(); ++i)
            {
                auto& s = state[i];
#endif
                // Warning: Unchecked removal.
                s.registers[id].value = 0;

                // FOR DEBUG
                /*s.registers[id] = s.last_register();
                s.last_register().value = 0;*/
            }
            remove_register(id);
        }

        System()
        { }

        inline bool operator<(const System & rhs) const
        {
            for (size_t i = 0; i < name_register_map.size(); ++i)
            {
                if (!status_of(i))
                    continue;
                auto& regl = registers[i];
                auto& regr = rhs.registers[i];
                if (regl < regr) return true;
                if (regr < regl) return false;
            }
            return false;
        }

        inline bool operator==(const System & rhs) const
        {
            for (size_t i = 0; i < name_register_map.size(); ++i)
            {
                if (!status_of(i))
                    continue;
                auto& regl = registers[i];
                auto& regr = rhs.registers[i];
                if (regl == regr)
                    continue;
                else
                    return false;
            }
            return true;
        }

        std::string to_string() const;
    };

    struct Register
    {
        int addr;
        bool local;

        Register(int addr, bool local): addr(addr), local(local) {}
    };

   struct Operation {
   protected:
       bool free;
       std::string name;
       std::shared_ptr<Register> target_bit;
       std::vector<std::shared_ptr<Register>> aux_bit;
       std::vector<std::shared_ptr<Register>> control_bit;
   public:

       Operation(bool free, const std::string &name, std::shared_ptr<Register> target, const std::vector<std::shared_ptr<Register>> &aux);

       virtual void some() {};

       const std::string &getName() const;

       const std::shared_ptr<Register> &getTargetBit() const;

       const std::vector<std::shared_ptr<Register>> &getAuxBit() const;

       const std::vector<std::shared_ptr<Register>> &getControlBit() const;

       void add_controls(std::vector<std::shared_ptr<Register>> controls);

   };

   struct Module;

   struct Function {
   protected:
       std::string name;
       std::vector<std::shared_ptr<Register>> targets;
       std::vector<std::shared_ptr<Register>> consts;
       std::vector<std::shared_ptr<Register>> control_bit;

   public:
       Function(const std::string &name, const std::vector<std::shared_ptr<Register>> &targets,
                const std::vector<std::shared_ptr<Register>> &consts);

       virtual void some() {}

       virtual void operator()(Module& mod) {};

       const std::string &getName() const;

       const std::vector<std::shared_ptr<Register>> &getTargets() const;

       const std::vector<std::shared_ptr<Register>> &getConsts() const;

       const std::vector<std::shared_ptr<Register>> &getControls() const;

       void add_controls(std::vector<std::shared_ptr<Register>> controls);

   };

   struct Module {
       std::vector<std::shared_ptr<Register>> regs;
       std::list<std::shared_ptr<Operation>> ops;
       std::stack<std::shared_ptr<Function>> funcs;

       Module() = default;
       Module(const Module& m) = default;
       Module(Module&& m) = default;

       auto append(Operation&& op) -> decltype(*this)
       {
           ops.emplace_back(std::make_shared<Operation>(op));
           return *this;
       }

       auto appendReg(Register& reg) -> decltype(*this)
       {
           regs.emplace_back(std::make_shared<Register>(reg));
           return *this;
       }

       auto operator|(Operation&& op) -> decltype(*this)
       {
           if (!funcs.empty())
               op.add_controls(funcs.top()->getControls());
           return append(std::forward<Operation>(op));
       }

       auto operator|(Function&& op) -> decltype(*this)
       {
           if (!funcs.empty())
               op.add_controls(funcs.top()->getControls());
           enter_scope(std::make_shared<Function>(op));
           op(*this);
           exit_scope();
           return *this;
       }

       auto operator<<(Register& reg) -> decltype(*this)
       {
           return appendReg(reg);
       }

       void enter_scope(std::shared_ptr<Function> func)
       {
           funcs.push(func);
       }

       void exit_scope()
       {
            funcs.pop();
       }

       void execute(std::vector<System>& state);

       void execute_dump(std::vector<System>& state);

       DAG modConvertToDAG();
   };

}; // namespace quint

#endif //QUINT_DATA_STRUCTURE_H

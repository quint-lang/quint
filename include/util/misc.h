//
// Created by BY210033 on 2023/3/15.
//

#ifndef QUINT_MISC_H
#define QUINT_MISC_H

#include <chrono>
#include <fstream>
#include <ctime>

#include "util/util.h"

namespace quint {

    constexpr double nanosec = 1;
    constexpr double microsec = nanosec / 1e3;
    constexpr double millisec = microsec / 1e3;
    constexpr double sec = millisec / 1e3;
    constexpr double minute = sec / 60;
    constexpr double hour = minute / 60;
    constexpr double day = hour / 24;

    template<typename Ty>
    std::string vec2str(std::vector<Ty> v, std::string lb = "[", std::string rb = "]", std::string sep = ",")
    {
        if (v.size() == 0) {
            return lb + rb;
        }
        std::stringstream ret;
        for (size_t i = 0; i < v.size() - 1; ++i) {
            ret << v[i] << sep;
        }
        ret << v.back();
        return lb + ret.str() + rb;
    }

    template<typename Ty>
    std::string num2str(Ty num) {
        return std::to_string(num);
    }

    std::string _datetime();

    std::string _datetime_simple();

    struct timer {
        std::chrono::time_point<std::chrono::steady_clock> startpoint;
        timer() {
            startpoint = std::chrono::steady_clock::now();
        }
        inline double get(double unit) {
            std::chrono::nanoseconds m = std::chrono::steady_clock::now() - startpoint;
            return std::chrono::duration_cast<std::chrono::nanoseconds>(m).count() * unit;
        }
    };

    struct FileLogger {
        std::ofstream out;
        bool on = true;
        std::vector<timer> timers;
        FileLogger() { }

        static FileLogger& instance()
        {
            static FileLogger static_logger;
            static bool init = false;
            if (!init) {
                static_logger.newfile(
                        autofilename("log-", ".txt")
                );
                static_logger.set_on();
                init = true;
            }
            return static_logger;
        }


        inline static std::string autofilename(std::string prefix, std::string postfix) {
            return prefix + _datetime_simple() + postfix;
        }

        void newfile_auto();

        inline void newfile(std::string name) {
            out = std::ofstream(name);
        }

        inline void set_on() { on = true; }
        inline void set_off() { on = false; }

        inline FileLogger& info(std::string str) {
            if (on)
                out << _datetime_simple()
                    << " [INFO] " << str << "\n";
            return *this;
        }

        inline FileLogger& error(std::string str) {
            if (on)
                out << _datetime_simple()
                    << " [ERROR] " << str << "\n";
            return *this;
        }

        inline FileLogger& operator<<(std::string str) {
            return info(str);
        }
        inline FileLogger& linesplit() {
            return info("----------------------------\n");
        }
        inline FileLogger& datetime() {
            return info(_datetime());
        }
        inline void timer_start() {
            timers.push_back(timer());
        }
        inline double timer_end(double unit = sec) {
            if (timers.size() == 0) {
                return 0.0;
            }
            double ret = timers.back().get(unit);
            timers.pop_back();
            return ret;
        }
        inline void flush() {
            out.flush();
        }
    };

    inline void FILE_INFO(std::string msg) {
        FileLogger::instance().info(msg);
    }

    inline void FILE_ERROR(std::string msg) {
        FileLogger::instance().error(msg);
    }

    struct profile {
        size_t ncalls = 0;
        double time = 0;
        std::vector<timer> timers;
        size_t max_depth = 100;
        profile() {
            enter();
        }
        void enter() {
            if (timers.size() == max_depth)
                throw std::runtime_error("Exceed max depth.");
            timers.push_back(timer());
            ncalls++;
        }
        void exit() {
            if (timers.size() == 0)
                throw std::runtime_error("Why profiler has 0 timer?");
            time += timers.back().get(millisec);
            timers.pop_back();
        }

    };

    inline std::string truncate_name(const std::string &name, size_t max_nchar)
    {
        std::string ret;
        ret.reserve(max_nchar);
        ret.assign(name.begin(), name.begin() + max_nchar - 3);
        ret += "...";
        return ret;
    }

    struct profiler {
        static std::map<std::string, profile*> profiles;
        static bool on;
        std::string current_identifier;
        profile* current_profile;

        profiler(std::string function_identifier) {
            if (!on) { return; }
            if (function_identifier.size() > 25) {
                current_identifier.assign(function_identifier.begin(), function_identifier.begin() + 15);
                current_identifier += "...";
            }
            else {
                current_identifier = function_identifier;
            }
            // current_identifier = function_identifier;

            auto iter = profiles.find(current_identifier);
            if (iter == profiles.end()) {
                current_profile = new profile();
                profiles.insert({ current_identifier, current_profile });
            }
            else {
                current_profile = iter->second;
                current_profile->enter();
            }
        }

        ~profiler() {
            if (!on) { return; }
            current_profile->exit();
        }

        inline static void init_profiler() {
            profiles.clear();
        }

        inline static void close_profiler() {
            profiler::on = false;
        }

        inline static void start_profiler() {
            profiler::on = true;
        }

        inline static double get_time(std::string profilename) {
            auto iter = profiles.find(profilename);
            if (iter == profiles.end()) return 0.0;
            else return iter->second->time;
        }

        inline static size_t get_ncalls(std::string profilename) {
            auto iter = profiles.find(profilename);
            if (iter == profiles.end()) return 0;
            else return iter->second->ncalls;
        }

        inline static std::string get_all_profiles() {
            if (profiles.empty()) {
                return "No profiles.";
            }
            std::string ret;
            for (const auto &profile : profiles) {
                ret += fmt::format("[{:^28s}] Calls = {:^3d} Time = {:^4f} ms\n",
                                   profile.first/*truncate_name(profile.first, 25)*/, profile.second->ncalls, profile.second->time);
            }
            return ret;
        }

        inline static std::vector<std::tuple<std::string, size_t, double>> get_profiles_info()
        {
            using profile_info_t = std::tuple<std::string, size_t, double>;
            std::vector<profile_info_t> ret;
            ret.reserve(profiles.size());

#if __cplusplus >= 201703L
            for (auto&& [name, profile] : profiles) {
                ret.emplace_back(name, profile->ncalls, profile->time);
            }
#else
            for (auto &name_profile : profiles) {
                auto& name = name_profile.first;
                auto& profile = name_profile.second;
                ret.emplace_back(name, profile->ncalls, profile->time);
            }
#endif

            std::sort(ret.begin(), ret.end(), [](const profile_info_t& lhs, const profile_info_t& rhs)
            {
                return std::get<2>(lhs) < std::get<2>(rhs);
            });

            return ret;
        }

        static std::string get_all_profiles_v2();
    };

#define FunctionProfiler volatile profiler _profilehelper_(__FUNCTION__)

    extern FileLogger file_logger;

    template <typename... Ty>
    void print_and_log(std::string fmt_str, Ty&&...args) {
        std::string str = format(fmt_str, std::forward<Ty>(args)...);
        fmt::print(str);
        file_logger << str;
        file_logger.flush();
    }

    template<typename Ty>
    struct Statistic
    {
        //std::string variable_name;
        std::vector<Ty> records;
        double sum = 0;
        double sum_sqr = 0;
        size_t shots = 0;

        inline Ty simple_record(Ty r) {
            double r_float = static_cast<double>(r);
            sum += r_float;
            sum_sqr += r_float * r_float;
            shots++;
            return r;
        }

        inline Ty record(Ty r) {
            double r_float = static_cast<double>(r);
            sum += r_float;
            sum_sqr += r_float * r_float;
            shots++;
            return r;
        }

        inline double mean() const
        {
            return sum / shots;
        }

        inline double mean_sqr() const
        {
            return sum_sqr / shots;
        }

        inline double Var() const
        {
            return mean_sqr() - mean() * mean();
        }

        inline double std() const
        {
            return std::sqrt(Var());
        }
    };

    using StatisticDouble = Statistic<double>;
    using StatisticInt = Statistic<int>;
    using StatisticSize = Statistic<size_t>;

}

#endif //QUINT_MISC_H

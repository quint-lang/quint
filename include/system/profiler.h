//
// Created by BY210033 on 2023/3/29.
//

#ifndef QUINT_PROFILER_H
#define QUINT_PROFILER_H

#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

#include "common/core.h"
#include "system/timer.h"

namespace quint {

    class ProfilerRecords;

    class ScopedProfiler {
    public:
        explicit ScopedProfiler(std::string name, uint64 elements = -1);

        void stop();

        static void enable();

        static void disable();

        ~ScopedProfiler();

    private:
        std::string name_;
        float64 start_time_;
        uint64 elements_;
        bool stopped_;
    };

    class Profiling {
    public:
        void print_profile_info();
        void clear_profile_info();
        ProfilerRecords *get_this_thread_profiler();
        static Profiling &get_instance();

    private:
        std::mutex mut_;
        std::unordered_map<std::thread::id, ProfilerRecords *> profilers_;
    };

#define QUINT_PROFILER(name) quint::ScopedProfiler _profiler_##__LINE__(name);

#define QUINT_AUTO_PROF QUINT_PROFILER(__FUNCTION__)

}

#endif //QUINT_PROFILER_H

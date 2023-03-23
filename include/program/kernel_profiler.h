//
// Created by BY210033 on 2023/3/15.
//

#ifndef QUINT_KERNEL_PROFILER_H
#define QUINT_KERNEL_PROFILER_H

#include "util/lang_util.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <regex>

namespace quint::lang {

    struct KernelProfileTraceRecord {
        int register_per_thread{0};
        int shared_mem_per_block{0};
        int grid_size{0};
        int block_size{0};
        int active_blocks_per_multiprocessor{0};
        // Kernel time
        float kernel_elapsed_time_in_ms{0.0};
        float time_since_base{0.0};
        std::string name;
        std::vector<float> metric_values;
    };

    struct KernelProfileStatisticalResult {
        std::string name;
        int counter;
        double min;
        double max;
        double total;

        explicit KernelProfileStatisticalResult(const std::string &name)
                : name(name), counter(0), min(0), max(0), total(0) {
        }

        void insert_record(double t);

        bool operator<(const KernelProfileStatisticalResult &other) const;
    };


    class KernelProfilerBase {
    protected:
        std::vector<KernelProfileTraceRecord> traced_records_;
        std::vector<KernelProfileStatisticalResult> statistical_results_;
        double total_time_ms_{0};

    public:
        using TaskHandle = void *;

        virtual bool reinit_with_metrics(const std::vector<std::string> metrics) {
            return false;
        };

        virtual void clear() = 0;

        virtual void sync() = 0;

        virtual void update() = 0;

        virtual bool set_profiler_toolkit(std::string toolkit_name) {
            return false;
        }

        virtual void start(const std::string &kernel_name) {
            QUINT_NOT_IMPLEMENTED
        }

        static void profiler_start(KernelProfilerBase *profiler, const char *kernel_name);

        virtual void stop() {
            QUINT_NOT_IMPLEMENTED
        }

        virtual void stop(TaskHandle) {
            QUINT_NOT_IMPLEMENTED
        }

        static void profiler_stop(KernelProfilerBase *profiler);

        void query(const std::string &kernel_name,
                   int &counter,
                   double &min,
                   double &max,
                   double &avg);

        std::vector<KernelProfileTraceRecord> get_traced_records() {
            return traced_records_;
        }

        double get_total_time() const;

        virtual std::string get_device_name() {
            std::string str("");
            return str;
        }

        virtual ~KernelProfilerBase() {}

    };

    std::unique_ptr<KernelProfilerBase> make_profiler(bool enable);

}

#endif //QUINT_KERNEL_PROFILER_H

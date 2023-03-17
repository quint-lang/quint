//
// Created by BY210033 on 2023/3/15.
//

#ifndef QUINT_KERNEL_PROFILER_H
#define QUINT_KERNEL_PROFILER_H

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

        virtual ~KernelProfilerBase() {}

    };

}

#endif //QUINT_KERNEL_PROFILER_H

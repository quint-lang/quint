//
// Created by BY210033 on 2023/3/22.
//
#include "program/kernel_profiler.h"
#include "system/timer.h"

namespace quint::lang {

    void KernelProfileStatisticalResult::insert_record(double t) {
        if (counter == 0) {
            min = t;
            max = t;
        }
        counter++;
        min = std::min(min, t);
        max = std::max(max, t);
        total += t;
    }

    bool KernelProfileStatisticalResult::operator<(const KernelProfileStatisticalResult &other) const {
        return total > other.total;
    }

    void KernelProfilerBase::profiler_start(KernelProfilerBase *profiler, const char *kernel_name) {
        QUINT_ASSERT(profiler)
        profiler->start(std::string(kernel_name));
    }

    void KernelProfilerBase::profiler_stop(KernelProfilerBase *profiler) {
        QUINT_ASSERT(profiler)
        profiler->stop();
    }

    void
    KernelProfilerBase::query(const std::string &kernel_name, int &counter, double &min, double &max, double &avg) {
        sync();
        std::regex name_regex(kernel_name + "(.*)");
        for (auto &rec : statistical_results_) {
            if (std::regex_match(rec.name, name_regex)) {
                if (counter == 0) {
                    counter = rec.counter;
                    min = rec.min;
                    max = rec.max;
                    avg = rec.total / rec.counter;
                } else if (counter == rec.counter) {
                    min += rec.min;
                    max += rec.max;
                    avg += rec.total / rec.counter;
                } else {
                    QUINT_WARN("{}.counter({}) != {}.counter({}).", kernel_name, counter,
                               rec.name, rec.counter);
                }
            }
        }
    }

    double KernelProfilerBase::get_total_time() const {
        return total_time_ms_ / 1000.0;
    }

    namespace {
        class DefaultProfiler : public KernelProfilerBase {
        public:
            void clear() override {
                total_time_ms_ = 0;
                traced_records_.clear();
                statistical_results_.clear();
            }

            void sync() override {

            }

            void update() override {

            }

            bool set_profiler_toolkit(std::string toolkit_name) override {
                return KernelProfilerBase::set_profiler_toolkit(toolkit_name);
            }

            void start(const std::string &kernel_name) override {
                start_t_ = Time::get_time();
                event_name_ = kernel_name;
            }

            void stop() override {
                auto t = Time::get_time() - start_t_;
                auto ms = t * 1000.0;
                // trace record
                KernelProfileTraceRecord record;
                record.name = event_name_;
                record.kernel_elapsed_time_in_ms = ms;
                traced_records_.push_back(record);
                // count record
                auto iter = std::find_if(statistical_results_.begin(), statistical_results_.end(),
                                         [&](KernelProfileStatisticalResult &r) {
                                             return r.name == event_name_;
                });
                if (iter == statistical_results_.end()) {
                    statistical_results_.emplace_back(event_name_);
                    iter = std::prev(statistical_results_.end());
                }
                iter->insert_record(ms);
                total_time_ms_ += ms;
            }

        private:
            double start_t_;
            std::string event_name_;
        };
    }

    std::unique_ptr<KernelProfilerBase> make_profiler(bool enable) {
        if (!enable)
            return nullptr;
        return std::make_unique<DefaultProfiler>();
    }

}
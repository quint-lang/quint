//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_PARALLEL_EXECUTOR_H
#define QUINT_PARALLEL_EXECUTOR_H

#include <mutex>
#include <deque>

#include "common/core.h"

namespace quint::lang {

    class ParallelExecutor {
    public:
        using TaskType = std::function<void()>;

        explicit ParallelExecutor(const std::string &name, int num_threads);
        ~ParallelExecutor();

        void enqueue(const TaskType &func);

        void flush();

        int get_num_threads() {
            return num_threads_;
        }

    private:
        enum class ExecutorStatus {
            uninitialized,
            initialized,
            finalized,
        };

        void  worker_loop();

        bool flush_cv_cond();

        std::string name_;
        int num_threads_;
        std::atomic<int> thread_counter_{0};
        std::mutex mut_;

        ExecutorStatus status_;
        std::vector<std::thread> threads_;
        std::deque<TaskType> task_queue_;
        int running_threads_;

        std::condition_variable init_cv_;
        std::condition_variable worker_cv_;
        std::condition_variable flush_cv_;
    };

}

#endif //QUINT_PARALLEL_EXECUTOR_H

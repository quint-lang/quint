//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_TIMELINE_H
#define QUINT_TIMELINE_H

#include "quint/common/core.h"
#include "quint/system/timer.h"

#include <vector>
#include <mutex>

namespace quint {

    struct TimelineEvent {
        std::string name;
        bool begin;
        float64 time;
        std::string tid;

        std::string to_json();
    };

    class Timeline {
    public:
        Timeline();
        ~Timeline();

        static Timeline &get_this_thread_instance();

        void set_name(const std::string &tid) {
            tid_ = tid;
        }

        std::string get_name() {
            return tid_;
        }

        void clear();

        void insert_event(const TimelineEvent &e);

        std::vector<TimelineEvent> fetch_events();

        class Guard {
        public:
            explicit Guard(const std::string &name);
            ~Guard();

        private:
            std::string name_;
        };

    private:
        std::string tid_;
        std::mutex mut_;
        std::vector<TimelineEvent> events_;
    };

    class Timelines {
    public:
        static Timelines &get_instance();

        void insert_events(const std::vector<TimelineEvent> &events);

        void insert_events_without_locking(const std::vector<TimelineEvent> &events);

        void insert_timeline(Timeline *timeline);

        void remove_timeline(Timeline *timeline);

        void clear();

        void save(const std::string &filename);

        bool get_enabled();

        void set_enabled(bool enabled);

    private:
        std::mutex mut_;
        std::vector<TimelineEvent> events_;
        std::vector<Timeline *> timelines_;
        bool enabled_{false};
    };

#define QUINT_TIMELINE(name) \
    quint::Timeline::Guard _timeline_guard_##__LINE__(name);
#define QUINT_AUTO_TIMELINE QUINT_TIMELINE(__FUNCTION__)
}

#endif //QUINT_TIMELINE_H

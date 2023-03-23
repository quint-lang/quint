//
// Created by BY210033 on 2023/3/22.
//

#ifndef QUINT_TIMER_H
#define QUINT_TIMER_H

#include <string>
#include <cstdio>
#include <map>

#include "common/core.h"
#if defined(QUINT_PLATORM_UNIX)
#include <sys/time.h>
#else
#pragma warning(push)
#pragma warning(disable : 4005)
#include "platform/windows/windows.h"
#pragma warning(pop)
#endif

namespace quint {

#define TIME(x)                                                          \
    {                                                                    \
        char timer_name[1000];                                           \
        sprintf_s(timer_name, "%s[%d]: %s", __FILENAME__, __LINE__, #x); \
        quint::Time::Timer _(timer_name);                                \
        x;                                                               \
    }
#define QUINT_TIME(x) TIME(x)

#include <stdint.h>

    class Time {
    public:
        static double get_time();

        class Timer {
            static std::map<std::string, std::pair<double, int>> memo;

        protected:
            std::string name;
            double start_time;
            bool have_output;

            virtual double get_time();

            virtual void print_record(const char *left, double elapsed, double average);

            void output();

        public:
            explicit Timer(std::string name);

            Timer() {
            }

            virtual ~Timer() {
                output();
            }
        };

        class TickTimer : public Timer {
        protected:

            double get_time() override;

            void print_record(const char *left, double elapsed, double average) override;

        public:
            explicit TickTimer(std::string name);

            ~TickTimer() override {
                output();
            }
        };
    };
}

#endif //QUINT_TIMER_H

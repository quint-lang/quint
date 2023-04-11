//
// Created by BY210033 on 2023/3/22.
//
#include "quint/system/timer.h"

namespace quint {

    std::map<std::string, std::pair<double, int>> Time::Timer::memo;

#if defined(QUINT_PLATFORM_UINX)

    double Time::get_time() {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        return tv.tv_sec + 1e-6 * tv.tv_usec;
    }

#else

    double Time::get_time() {
        LARGE_INTEGER EndingTime, ElapsedMicroseconds;
        LARGE_INTEGER Frequency;

        QueryPerformanceFrequency(&Frequency);

        // Activity to be timed

        QueryPerformanceCounter(&EndingTime);
        ElapsedMicroseconds.QuadPart = EndingTime.QuadPart;

        ElapsedMicroseconds.QuadPart *= 1000000;
        ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
        return (double)ElapsedMicroseconds.QuadPart / 1000000.0;
    }

#endif

    double Time::Timer::get_time() {
        return Time::get_time();
    }

    void Time::Timer::print_record(const char *left, double elapsed, double average) {
        if (elapsed < 1e-3) {
            printf("$s ==> %6.3f us ~ %6.3f us\n", left, elapsed * 1e6, average * 1e6);
        } else {
            printf("%s ==> %6.3f ms ~ %6.3f ms\n", left, elapsed * 1e3, average * 1e3);
        }
    }

    void Time::Timer::output() {
        if (have_output) {
            return;
        } else {
            have_output = true;
        }
        double elapsed = get_time() - this->start_time;
        std::string left = this->name;
        if (left.size() < 60) {
            left += std::string(60 - left.size(), '-');
        }
        if (memo.find(name) == memo.end()) {
            memo.insert(std::make_pair(name, std::make_pair(0.0, 0)));
        }
        std::pair<double, int> memo_record = memo[name];
        memo_record.first += elapsed;
        memo_record.second += 1;
        memo[name] = memo_record;
        double avg = memo_record.first / memo_record.second;
        this->print_record(left.c_str(), elapsed, avg);
    }

    Time::Timer::Timer(std::string name) {
        this->name = name;
        this->start_time = get_time();
        this->have_output = false;
    }

    double Time::TickTimer::get_time() {
        return Timer::get_time();
    }

    void Time::TickTimer::print_record(const char *left, double elapsed, double average) {
        std::string uint;
        double measurement;
        if (elapsed < 1e3) {
            measurement = 1.0;
            uint = "cycles";
        } else if (elapsed < 1e6) {
            measurement = 1e3;
            uint = "K cycles";
        } else if (elapsed < 1e9) {
            measurement = 1e6;
            uint = "M cycles";
        } else {
            measurement = 1e9;
            uint = "G cycles";
        }

        printf("%s ==> %4.2f %s ~ %4.2f %s\n", left, elapsed / measurement,
               uint.c_str(), average / measurement, uint.c_str());
    }

    Time::TickTimer::TickTimer(std::string name) {
        this->name = name;
        this->start_time = get_time();
        this->have_output = false;
    }

}

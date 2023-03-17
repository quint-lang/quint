//
// Created by BY210033 on 2023/2/7.
//
#include "common/logger.h"

#include "spdlog/common.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

using namespace std;

namespace quint {

    const auto default_logging_level = "info";

    Logger::Logger() {
        console_ = spdlog::stdout_color_mt("console");
        console_->flush_on(spdlog::level::trace);
        QUINT_LOG_SET_PATTERN("%^[%L %D %X.%e %t] %v%$");

        set_level_default();
    }

    void Logger::trace(const string &s) {
        console_->trace(s);
    }

    void Logger::debug(const string &s) {
        console_->debug(s);
    }

    void Logger::info(const string &s) {
        console_->info(s);
    }

    void Logger::warn(const string &s) {
        console_->warn(s);
    }

    void Logger::error(const string &s, bool raise_exception) {
        console_->error(s);
        fmt::print("\n\n");
        if (print_stacktrace_fn_) {
            print_stacktrace_fn_();
        }
//        if ()
        if (raise_exception)
            throw s;
    }

    void Logger::critical(const string &s) {
        Logger::error(s);
    }

    void Logger::flush() {
        console_->flush();
    }

    void Logger::set_level(const string &level) {
        auto new_level = level_enum_form_string(level);
        level_ = new_level;
        spdlog::set_level((spdlog::level::level_enum)level_);
    }

    bool Logger::is_level_effective(const string &level_name) {
        return get_level() <= level_enum_form_string(level_name);
    }

    int Logger::get_level() {
        return level_;
    }

    int Logger::level_enum_form_string(const string &level) {
        if (level == "trace") {
            return spdlog::level::trace;
        } else if (level == "debug") {
            return spdlog::level::debug;
        } else if (level == "info") {
            return spdlog::level::info;
        } else if (level == "warn") {
            return spdlog::level::warn;
        } else if (level == "error") {
            return spdlog::level::err;
        } else if (level == "critical") {
            return spdlog::level::critical;
        } else if (level == "off") {
            return spdlog::level::off;
        } else {
            QUINT_ERROR("Unknown logging level [{}]. Levels = trace, debug, info, warn, error, "
                      "critical off", level);
        }
    }

    void Logger::set_level_default() {
        set_level(default_logging_level);
    }

    void Logger::set_print_stacktrace_func(std::function<void()> print_fn) {
        print_stacktrace_fn_ = print_fn;
    }

    Logger &Logger::get_instance() {
        static Logger *l = new Logger();
        return *l;
    }

} // namespace quint
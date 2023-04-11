//
// Created by BY210033 on 2023/2/7.
//

#ifndef QUINT_LOGGER_H
#define QUINT_LOGGER_H

#include "quint/common/platform_macros.h"

#define FMT_HEADER_ONLY
#include "spdlog/common.h"
#include "spdlog/fmt/fmt.h"

namespace spdlog {
    class logger;
}

#ifdef _WIN64
#define __FILENAME__ \
  (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define SPD_AUGMENTED_LOG(X, ...)                                        \
  quint::Logger::get_instance().X(                                         \
      fmt::format("[{}:{}@{}] ", __FILENAME__, __FUNCTION__, __LINE__) + \
      fmt::format(__VA_ARGS__))

#if defined(QUINT_PLATFORM_WINDOWS)
#define QUINT_UNREACHABLE __assume(0);
#else
#define QUINT_UNREACHABLE __builtin_unreachable();
#endif

#define QUINT_TRACE(...)  SPD_AUGMENTED_LOG(trace, __VA_ARGS__)
#define QUINT_DEBUG(...)  SPD_AUGMENTED_LOG(debug, __VA_ARGS__)
#define QUINT_INFO(...)   SPD_AUGMENTED_LOG(info, __VA_ARGS__)
#define QUINT_WARN(...)   SPD_AUGMENTED_LOG(warn, __VA_ARGS__)
#define QUINT_ERROR(...)                          \
    {                                           \
         SPD_AUGMENTED_LOG(error, __VA_ARGS__); \
         QUINT_UNREACHABLE;                       \
    }
#define QUINT_CRITICAL(...)                          \
    {                                              \
        SPD_AUGMENTED_LOG(critical, __VA_ARGS__);  \
        QUINT_UNREACHABLE                            \
    }

#define QUINT_ERROR_UNLESS(condition, ...) \
  if (!(condition)) {                      \
    QUINT_ERROR(__VA_ARGS__);              \
  }

#define QUINT_ERROR_IF(condition, ...) \
  if (condition) {                     \
    QUINT_ERROR(__VA_ARGS__);          \
  }

#define QUINT_ASSERT(x)   QUINT_ASSERT_INFO((x), "Assertion failure" #x)
#define QUINT_ASSERT_INFO(x, ...)             \
{                                           \
    bool ___ret___ = static_cast<bool>(x);  \
    if (!___ret___) {                       \
        QUINT_ERROR(__VA_ARGS__);             \
    }                                       \
}

#define QUINT_NOT_IMPLEMENTED QUINT_ERROR("Not supported.");

#define QUINT_LOG_SET_PATTERN(x) spdlog::set_pattern(x);

#define QUINT_P(x) \
  { QUINT_INFO("{}", quint::TextSerializer::serialize(#x, (x))); }

namespace quint {

    class Logger {
    private:
        std::shared_ptr<spdlog::logger> console_;
        int level_;
        std::function<void()> print_stacktrace_fn_;

        Logger();

    public:
        void trace(const std::string &s);
        void debug(const std::string &s);
        void info(const std::string &s);
        void warn(const std::string &s);
        void error(const std::string &s, bool raise_exception = true);
        void critical(const std::string &s);
        void flush();
        void set_level(const std::string &level);
        bool is_level_effective(const std::string &level_name);
        int get_level();
        static int level_enum_form_string(const std::string &level);
        void set_level_default();

        void set_print_stacktrace_func(std::function<void()> print_fn);

        static Logger &get_instance();
    };


} // namespace quint

#endif //QUINT_LOGGER_H

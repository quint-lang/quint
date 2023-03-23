//
// Created by BY210033 on 2023/3/14.
//
#include "common/core.h"

namespace quint {

    std::string python_package_dir;

    int __trash__;

    CoreState &CoreState::get_instance() {
        static CoreState state;
        return state;
    }

    std::string get_repo_dir() {
#if defined(QUINT_PLATFORM_WINDOWS)
        return "C:/quint_cache/";
#else
        auto xdg_cache = std::getenv("XDG_CACHE_HOME");

        std::string xdg_cache_str;
        if (xdg_cache != nullptr) {
            xdg_cache_str = xdg_cache;
        } else {
            auto home = std::getenv("HOME");
            QUINT_ASSERT(home != nullptr);
            xdg_cache_str = home;
            xdg_cache_str += "/.cache";
        }

        return xdg_cache_str + "/quint/";
#endif
    }

    std::string get_python_package_dir() {
        return python_package_dir;
    }

    void set_python_package_dir(const std::string &dir) {
        python_package_dir = dir;
    }

}
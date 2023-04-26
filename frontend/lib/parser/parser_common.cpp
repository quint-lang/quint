//
// Created by BY210033 on 2023/4/17.
//
#include "parser/parser_common.h"
#include <cstdlib>
#include <cstdio>
#include <climits>
#ifdef WIN32
#include <io.h>
#include <tchar.h>
#include <direct.h>
#include <windows.h>
#endif

namespace quint::ast {

    std::string getAbsolutePath(const std::string &path) {
#ifdef WIN32
#define max_path 4096
        char resolved_path[max_path]={0};
        _fullpath(resolved_path, path.c_str(), max_path);
#else
        #define max_path 40960
        char resolved_path[max_path]={0};
        realpath(path.c_str(), resolved_path);
#endif
        return std::string(resolved_path);
    }

}
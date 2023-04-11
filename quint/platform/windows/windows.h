//
// Created by BY210033 on 2023/3/14.
//

#ifndef QUINT_WINDOWS_H
#define QUINT_WINDOWS_H

#include <windows.h>
// Never directly include <windows.h>. That will bring you evil max/min macros.
#if defined(min)
#undef min
#endif
#if defined(max)
#undef max
#endif


#endif //QUINT_WINDOWS_H

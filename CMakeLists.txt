cmake_minimum_required(VERSION 3.20)

project(quint)

#include_directories(${PROJECT_SOURCE_DIR}/include
#        ${PROJECT_SOURCE_DIR}/tpl
#        ${PROJECT_SOURCE_DIR}/tpl/argparse
#        ${PROJECT_SOURCE_DIR}/tpl/eigen
#        ${PROJECT_SOURCE_DIR}/tpl/spdlog/include
#        ${PROJECT_SOURCE_DIR}/GPU/include
#        )

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/lib)
#set(CMAKE_CXX_FLAGS "-g -O0")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
option(CMAKE_BUILD_TYPE "Release")

find_package(OpenMP REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

find_package(CUDA)
if (CUDA_FOUND)
    message("CUDA Found.")
    include_directories(CUDA_INCLUDE_DIRS)
    message("CUDA include dirs = ${CUDA_INCLUDE_DIRS}")
    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_STANDARD_REQUIRED TRUE)
endif()

if (UNIX)
    add_compile_options(-fPIC)
endif(UNIX)

option(QUINT_WITH_PYTHON "Build with Python language binding" ON)
if (QUINT_WITH_PYTHON)
    include(cmake/PythonNumpyPybind11.cmake)
endif ()

# Setup CLANG_EXECUTABLE
if (CLANG_EXECUTABLE)
#    set(CLANG_EXECUTABLE "F:/llvm-15/bin/clang.exe")
    message("CLANG_EXECUTABLE defined: ${CLANG_EXECUTABLE}")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set (CLANG_EXECUTABLE ${CMAKE_CXX_COMPILER})
    message("Clang executable using host compiler ${CLANG_EXECUTABLE}")
else()
#    set(CLANG_EXECUTABLE "F:/llvm-15/bin/clang.exe")
    find_program(CLANG_EXECUTABLE NAMES clang-15 clang-14 clang-13 clang-12 clang-11 clang-10 clang-9 clang-8 clang-7 clang)
    message("Clang executable found at ${CLANG_EXECUTABLE}")
endif()

if (NOT CLANG_EXECUTABLE)
    message(FATAL_ERROR "Cannot find any clang executable.")
endif()

macro(check_clang_version)
    execute_process(COMMAND ${CLANG_EXECUTABLE} --version OUTPUT_VARIABLE CLANG_VERSION_OUTPUT)
    string(REGEX MATCH "([0-9]+)\\.[0-9]+(\\.[0-9]+)?" CLANG_VERSION "${CLANG_VERSION_OUTPUT}")
    message("${CLANG_EXECUTABLE} --version: ${CLANG_VERSION}")

    set(CLANG_VERSION_MAJOR "${CMAKE_MATCH_1}")
endmacro()

if (APPLE)
    set(CLANG_OSX_FLAGS "-isysroot${CMAKE_OSX_SYSROOT}")
endif()


include(cmake/QuintCXXFlags.cmake)
include(cmake/QuintCore.cmake)

if (QUINT_WITH_LLVM)
    add_subdirectory(quint/runtime/llvm/runtime_module)
endif ()

#add_subdirectory(mlir)
add_subdirectory(tpl EXCLUDE_FROM_ALL)
add_subdirectory(test)


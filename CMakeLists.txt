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

include(cmake/QuintCore.cmake)

add_subdirectory(mlir)
add_subdirectory(tpl EXCLUDE_FROM_ALL)
add_subdirectory(test)


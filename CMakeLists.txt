cmake_minimum_required(VERSION 3.13.4)
project(quint)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_CXX_STANDARD 17)

set(LLVM_DIR "F://llvm-12//lib//cmake//llvm")

if (POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif ()

find_package(LLVM REQUIRED HINTS "${LLVM_INCLUDE_DIR}")
if (LLVM_FOUND)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

    list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})
    include(ChooseMSVCCRT)

    include(AddLLVM)
    include(HandleLLVMOptions)

    include_directories("${LLVM_BINARY_DIR}/include" "${LLVM_INCLUDE_DIR}")
    link_directories(${LLVM_LIBRARY_DIR})
endif ()

find_package(MLIR REQUIRED CONFIG)
if (MLIR_FOUND)
    message(STATUS "Using MLIRConfig.cmake in: ${MLIR_DIR}")

    list(APPEND CMAKE_MODULE_PATH ${MLIR_CMAKE_DIR})
    include(TableGen)
    include(AddMLIR)

    include_directories(${MLIR_INCLUDE_DIRS})
endif ()

set(QUINT_BUILT_STANDALONE 1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

set(QUINT_VERSION_STRING "0.1")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/version.inc.in
    ${CMAKE_CURRENT_BINARY_DIR}/src/version.inc
)

include_directories(include)

add_subdirectory(compiler)
add_subdirectory(src)
add_subdirectory(tpl EXCLUDE_FROM_ALL)
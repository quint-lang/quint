cmake_minimum_required(VERSION 3.10)

project (QuintDialect LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to conform to")

set(LLVM_DIR "F://llvm-12//lib//cmake//llvm")

if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

if (POLICY CMP0116)
    cmake_policy(SET CMP0116 NEW)
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

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_BINARY_DIR}/include)

add_subdirectory(include)
add_subdirectory(lib)

file(GLOB SRC quint_mlir_api.cpp)

get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)

set(LIBS
        ${dialect_libs}
        ${conversion_libs}
        MLIRQuint
        MLIRTargetLLVMIR
        MLIRExecutionEngine
        MLIRStandard
        MLIRAffine
        LLVMLinker
        )

add_mlir_library(qcor-mlir-api SHARED ${SRC} LINK_LIBS PUBLIC ${LIBS})

target_include_directories(
        qcor-mlir-api PUBLIC .
        include/dialect
        ${CMAKE_BINARY_DIR}/mlir/include/dialect)
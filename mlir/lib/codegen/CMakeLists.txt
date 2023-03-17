file(GLOB_RECURSE MLIRGEN_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_llvm_library(mlir-gen ${MLIRGEN_SRCS})

target_include_directories(mlir-gen PUBLIC .
                            ../../include
                            ${CMAKE_BINARY_DIR}/mlir/include)

target_link_libraries(mlir-gen PUBLIC
                                MLIRQuint
                                MLIRSCF
                                MLIRIR)

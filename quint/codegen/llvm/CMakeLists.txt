file(GLOB LLVM_CODEGEN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(llvm_codegen ${LLVM_CODEGEN_SRC})

target_include_directories(llvm_codegen
        PRIVATE
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/tpl/eigen
        ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(llvm_codegen PRIVATE quint_util)
target_link_libraries(llvm_codegen PRIVATE llvm_runtime)
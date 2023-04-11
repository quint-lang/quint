add_library(cpu_codegen)
target_sources(cpu_codegen
        PRIVATE
        codegen_cpu.cpp
)

target_include_directories(cpu_codegen
        PRIVATE
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/tpl/eigen
        ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(cpu_codegen PRIVATE quint_util)
target_link_libraries(cpu_codegen PRIVATE cpu_runtime)
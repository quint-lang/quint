add_library(llvm_program_impl)
target_sources(llvm_program_impl
        PRIVATE
        llvm_program.cpp
)

target_include_directories(llvm_program_impl
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/tpl/eigen
        ${PROJECT_SOURCE_DIR}/tpl/spdlog/include
        ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(llvm_program_impl PRIVATE cpu_runtime)
target_link_libraries(llvm_program_impl PRIVATE llvm_codegen)
file(GLOB CPU_RUNTIME_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(cpu_runtime ${CPU_RUNTIME_SRC})

target_include_directories(cpu_runtime
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/tpl/eigen
        ${PROJECT_SOURCE_DIR}/tpl/spdlog/include
        ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(cpu_runtime PRIVATE cpu_rhi)

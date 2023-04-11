set(CPU_RHI cpu_rhi)

file(GLOB CPU_RHI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(${CPU_RHI} ${CPU_RHI_SRC})


target_include_directories(${CPU_RHI}
        PRIVATE
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/tpl/eigen
        ${PROJECT_SOURCE_DIR}/tpl/spdlog/include
)
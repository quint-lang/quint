file(GLOB LLVM_RUNTIME_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(llvm_runtime ${LLVM_RUNTIME_SRC})

target_include_directories(llvm_runtime
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/tpl/eigen
        ${PROJECT_SOURCE_DIR}/tpl/spdlog/include
        ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(llvm_runtime PRIVATE llvm_rhi)
target_link_libraries(llvm_runtime PRIVATE ${llvm_libs})

if (APPLE AND "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
    target_link_libraries(llvm_runtime PRIVATE ${llvm_aarch64_libs})
endif()


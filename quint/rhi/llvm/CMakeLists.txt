set(LLVM_RHI llvm_rhi)
file(GLOB LLVM_RHI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(${LLVM_RHI} ${LLVM_RHI_SRC})

target_include_directories(${LLVM_RHI}
        PRIVATE
        ${PROJECT_SOURCE_DIR}
        )


target_link_libraries(${LLVM_RHI} PRIVATE quint_util)
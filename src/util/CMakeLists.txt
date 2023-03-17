file(GLOB QUINT_UTIL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(quint_util ${QUINT_UTIL_SRC})

target_include_directories(quint_util
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/tpl/eigen
        ${PROJECT_SOURCE_DIR}/tpl/spdlog/include
        ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(quint_util PUBLIC quint_common)
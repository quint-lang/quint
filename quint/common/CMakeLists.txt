file(GLOB QUINT_COMMON_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(quint_common ${QUINT_COMMON_SRC})

target_include_directories(quint_common
        PUBLIC
        ${PROJECT_SOURCE_DIR}/tpl/spdlog/include
        PRIVATE
        ${PROJECT_SOURCE_DIR}
        )
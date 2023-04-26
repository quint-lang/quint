file(GLOB_RECURSE QUINT_STANDALONE_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(quint ${QUINT_STANDALONE_SRCS})

target_include_directories(quint PUBLIC
                                 ${CMAKE_CURRENT_SOURCE_DIR}/include
                                 ${PROJECT_SOURCE_DIR}
                                 ${PROJECT_SOURCE_DIR}/tpl/spdlog/include)

target_link_libraries(quint quint_core)
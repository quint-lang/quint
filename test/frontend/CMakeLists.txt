add_executable(FrontedLoggerTest FrontedLoggerTest.cpp)
target_include_directories(FrontedLoggerTest PRIVATE ${PROJECT_SOURCE_DIR}/tpl/spdlog/include)
add_executable(LexerTest LexerTest.cpp)
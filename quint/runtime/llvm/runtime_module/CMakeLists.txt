function(COMPILE_LLVM_RUNTIME rtm_arch)
    message(STATUS "Compiling LLVM byte code file for arch ${rtm_arch}")
    message("clang IS " ${CLANG_EXECUTABLE})
    message("clang osx flags is " ${CLANG_OSX_FLAGS})
    add_custom_target(
            "generate_llvm_runtime_${rtm_arch}"
            COMMAND ${CLANG_EXECUTABLE} ${CLANG_OSX_FLAGS} -c runtime.cpp -o "runtime_${rtm_arch}.bc" -fno-exceptions -emit-llvm -std=c++17 -D "ARCH_${rtm_arch}"  -I ${PROJECT_SOURCE_DIR};
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    add_dependencies(${CORE_LIBRARY_NAME} "generate_llvm_runtime_${rtm_arch}")
    install(FILES "${CMAKE_SOURCE_DIR}/src/runtime/llvm/runtime_module/runtime_${arch}.bc" DESTINATION ${CMAKE_INSTALL_PREFIX}/python/quint/_lib/runtime)
endfunction()

foreach(arch IN LISTS HOST_ARCH CUDA_ARCH DX12_ARCH)
    compile_llvm_runtime(${arch})
endforeach()

#add_library(qr_runtime runtime.cpp)
#target_include_directories(qr_runtime PRIVATE ${PROJECT_SOURCE_DIR}/include)
#target_include_directories(qr_runtime PRIVATE ${PROJECT_SOURCE_DIR}/tpl/eigen)
#target_include_directories(qr_runtime PRIVATE ${PROJECT_SOURCE_DIR}/tpl/argparse)
#target_include_directories(qr_runtime PRIVATE ${PROJECT_SOURCE_DIR}/tpl/spdlog/include)
#target_include_directories(qr_runtime PRIVATE "${LLVM_BINARY_DIR}/include" "${LLVM_INCLUDE_DIR}")
#target_link_libraries(qr_runtime quint_core)
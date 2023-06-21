option(QUINT_WITH_LLVM "Build with LLVM backends" ON)
option(QUINT_WITH_CUDA "Build with the CUDA backend" OFF)
option(QUINT_WITH_CUDA_TOOLKIT "Build with the CUDA toolkit" OFF)
option(QUINT_WITH_QPANDA "Build the QPanda backend" OFF)

if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

if (POLICY CMP0116)
    cmake_policy(SET CMP0116 NEW)
endif ()

set(INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/python/quint/_lib)

file(GLOB QUINT_CORE_SRC
    "quint/ir/*.cpp"
    "quint/jit/*.cpp"
    "quint/system/*.cpp"
    "quint/program/*.cpp"
    "quint/transforms/*.cpp"
    "quint/runtime/hvm/*.cpp"
#    "quint/runtime/simulate/*.cpp"
    "quint/codegen/*.cpp"
    "quint/rhi/*.cpp"
    "quint/analysis/*.cpp"
    "quint/struct/*.cpp"
)

if(QUINT_WITH_LLVM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQUINT_WITH_LLVM")
endif()

set(CORE_LIBRARY_NAME quint_core)
add_library(${CORE_LIBRARY_NAME} OBJECT ${QUINT_CORE_SRC})

if (APPLE)
    set_target_properties(${CORE_LIBRARY_NAME}
            PROPERTIES INTERFACE_LINK_LIBRARIES "-undefined dynamic_lookup"
    )
endif ()

target_include_directories(${CORE_LIBRARY_NAME} PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(${CORE_LIBRARY_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/tpl/eigen)
target_include_directories(${CORE_LIBRARY_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/tpl/argparse)
target_include_directories(${CORE_LIBRARY_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/tpl/spdlog/include)
target_include_directories(${CORE_LIBRARY_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/tpl/PicoSHA2)

if (QUINT_WITH_LLVM)
    set(LLVM_DIR "F://llvm-15//lib//cmake//llvm")
    message("Getting LLVM_DIR=${LLVM_DIR} from the environment variable")

    find_package(LLVM REQUIRED HINTS "${LLVM_INCLUDE_DIR}")
    if (LLVM_FOUND)
        message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
        message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

        list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})
        include(ChooseMSVCCRT)

        include(AddLLVM)
        include(HandleLLVMOptions)

#        include_directories("${LLVM_BINARY_DIR}/include" "${LLVM_INCLUDE_DIR}")
        link_directories(${LLVM_LIBRARY_DIR})
        add_definitions(${LLVM_DEFINITIONS})

        target_include_directories(${CORE_LIBRARY_NAME} PUBLIC ${LLVM_INCLUDE_DIRS})

        llvm_map_components_to_libnames(llvm_libs
                Core
                ExecutionEngine
                InstCombine
                OrcJIT
                RuntimeDyld
                TransformUtils
                BitReader
                BitWriter
                Object
                ScalarOpts
                Support
                native
                Linker
                Target
                MC
                Passes
                ipo
                Analysis
                )

        if (APPLE AND "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
            llvm_map_components_to_libnames(llvm_aarch64_libs AArch64)
        endif()

        add_subdirectory(quint/codegen/cpu)
        add_subdirectory(quint/runtime/cpu)
        add_subdirectory(quint/rhi/cpu)

        target_link_libraries(${CORE_LIBRARY_NAME} PRIVATE cpu_codegen)
        target_link_libraries(${CORE_LIBRARY_NAME} PRIVATE cpu_runtime)
        target_link_libraries(${CORE_LIBRARY_NAME} PRIVATE cpu_rhi)

        add_subdirectory(quint/rhi/llvm)
        add_subdirectory(quint/codegen/llvm)
        add_subdirectory(quint/runtime/llvm)
        add_subdirectory(quint/runtime/program_impls/llvm)

        target_link_libraries(${CORE_LIBRARY_NAME} PRIVATE llvm_program_impl)
        target_link_libraries(${CORE_LIBRARY_NAME} PRIVATE llvm_codegen)
        target_link_libraries(${CORE_LIBRARY_NAME} PRIVATE llvm_runtime)

        if (LINUX)
            # Remove symbols from llvm static libs
            foreach(LETTER ${llvm_libs})
                target_link_options(${CORE_LIBRARY_NAME} PUBLIC -Wl,--exclude-libs=lib${LETTER}.a)
            endforeach()
        endif()
    endif ()
endif ()

add_subdirectory(quint/util)
add_subdirectory(quint/common)

target_link_libraries(${CORE_LIBRARY_NAME} PRIVATE quint_util)
target_link_libraries(${CORE_LIBRARY_NAME} PRIVATE quint_common)

if (QUINT_WITH_PYTHON)
    message("PYTHON_LIBRARIES: " ${PYTHON_LIBRARIES})
    set(CORE_WITH_PYBIND_LIBRARY_NAME quint_python)
    file(GLOB QUINT_PYBIND_SOURCE
            "quint/python/*.cpp"
            "quint/python/*.h"
    )

    pybind11_add_module(${CORE_WITH_PYBIND_LIBRARY_NAME} NO_EXTRAS ${QUINT_PYBIND_SOURCE})

    message("Build Lib: " ${CORE_WITH_PYBIND_LIBRARY_NAME})

    if (LINUX)
        target_link_options(${CORE_WITH_PYBIND_LIBRARY_NAME} PUBLIC -Wl,--exclude-libs=ALL)
    endif()

    target_link_libraries(${CORE_WITH_PYBIND_LIBRARY_NAME} PRIVATE ${CORE_LIBRARY_NAME})

    target_include_directories(${CORE_WITH_PYBIND_LIBRARY_NAME}
            PRIVATE
            ${PROJECT_SOURCE_DIR}
            ${PROJECT_SOURCE_DIR}/tpl/spdlog/include
            ${PROJECT_SOURCE_DIR}/tpl/eigen
            )

    # These commands should apply to the DLL that is loaded from python, not the OBJECT library.
    if (MSVC)
        set_property(TARGET ${CORE_WITH_PYBIND_LIBRARY_NAME} APPEND PROPERTY LINK_FLAGS /DEBUG)
    endif ()

    if (WIN32)
        set_target_properties(${CORE_WITH_PYBIND_LIBRARY_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                "${CMAKE_CURRENT_SOURCE_DIR}/runtimes")
    endif ()

    install(TARGETS ${CORE_WITH_PYBIND_LIBRARY_NAME}
            RUNTIME DESTINATION ${INSTALL_LIB_DIR}/core
            LIBRARY DESTINATION ${INSTALL_LIB_DIR}/core)
endif ()

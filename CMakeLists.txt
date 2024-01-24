cmake_minimum_required(VERSION 3.27)
project(mlir_toy)

set(CMAKE_CXX_STANDARD 23)


set(LLVM_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../llvm-project")
set(LLVM_BUILD_PATH "${LLVM_SRC_PATH}/build")
set(MLIR_DIR "${LLVM_BUILD_PATH}/lib/cmake/mlir" CACHE PATH "")
unset(LLVM_DIR CACHE)

if (NOT DEFINED MLIR_DIR OR "${MLIR_DIR}" STREQUAL "MLIR_DIR-NOTFOUND" OR NOT EXISTS "${MLIR_DIR}")
    message(WARNING "MLIR_DIR is not configured so we need build mlir at configure time."
            "Set the cmake option MLIR_DIR for prebuilt MLIR, e.g.,\n"
            "    cmake -DMLIR_DIR=/path/to/llvm-project/build/lib/cmake/mlir ..\n"
    )
    if(NOT EXISTS "${LLVM_SRC_PATH}")
        message(FATAL_ERROR "LLVM_SRC_PATH=${LLVM_SRC_PATH} does not exist."
                "Set the cmake option LLVM_SRC_PATH for LLVM, e.g.,\n"
                "    cmake -DLLVM_SRC_PATH=/path/to/llvm-project ..\n"
        )
    endif()

    # build MLIR at configure time
    function(build_external_mlir)
        find_package(pybind11 REQUIRED)
        execute_process(COMMAND ${CMAKE_COMMAND}
                -S "${LLVM_SRC_PATH}/llvm"
                -B "${LLVM_BUILD_PATH}"
                -G "${CMAKE_GENERATOR}"
                -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                -DCMAKE_BUILD_TYPE=Release
                -DLLVM_ENABLE_PROJECTS=mlir
                -DLLVM_TARGETS_TO_BUILD=X86\;NVPTX\;AMDGPU
                -DLLVM_ENABLE_ASSERTIONS=ON
                -DMLIR_ENABLE_BINDINGS_PYTHON=ON

                RESULT_VARIABLE result
                WORKING_DIRECTORY ${LLVM_SRC_PATH}
        )

        if(result)
            message(FATAL_ERROR "CMake step for llvm failed: ${result}")
        endif()

        execute_process(
                COMMAND ${CMAKE_COMMAND} --build ${LLVM_BUILD_PATH}
                RESULT_VARIABLE result
                WORKING_DIRECTORY ${LLVM_SRC_PATH}
        )

        if(result)
            message(FATAL_ERROR "Build step for llvm failed: ${result}")
        endif()

        set(MLIR_DIR "${LLVM_BUILD_PATH}/lib/cmake/mlir" CACHE PATH "" FORCE)
    endfunction(build_external_mlir)
    build_external_mlir()
endif()


find_package(MLIR REQUIRED CONFIG)
message(STATUS "Using MLIRConfig.cmake in: ${MLIR_DIR}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(LLVM_RUNTIME_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/bin)
set(LLVM_LIBRARY_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/lib)
set(MLIR_BINARY_DIR ${CMAKE_BINARY_DIR})

#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH "${MLIR_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")

include(TableGen)
include(AddLLVM)
include(AddMLIR)
include(HandleLLVMOptions)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${MLIR_INCLUDE_DIRS})
link_directories(${LLVM_BUILD_LIBRARY_DIR})
add_definitions(${LLVM_DEFINITIONS})

add_executable(mlir_toy main.cpp
        src/toy_dialect.h)
target_link_libraries(mlir_toy PUBLIC MLIRDialect)

include_directories(src)


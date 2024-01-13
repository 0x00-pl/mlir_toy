cmake_minimum_required(VERSION 3.27)
project(mlir_toy)

set(CMAKE_CXX_STANDARD 23)

add_executable(mlir_toy main.cpp)

set(LLVM_BUILD_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../llvm-project/build")
set(MLIR_DIR "${LLVM_BUILD_PATH}/lib/cmake/mlir" CACHE PATH "" FORCE)
find_package(MLIR REQUIRED CONFIG)


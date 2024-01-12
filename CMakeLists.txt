cmake_minimum_required(VERSION 3.27)
project(mlir_toy)

set(CMAKE_CXX_STANDARD 23)

add_executable(mlir_toy main.cpp)

find_package(LLVM 16.0.6 REQUIRED CONFIG)

target_link_libraries(mlir_toy PRIVATE LLVM)

install(TARGETS mlir_toy RUNTIME DESTINATION bin)


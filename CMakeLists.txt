cmake_minimum_required(VERSION 3.10.0)
project(riscv-emu VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)

add_executable(riscv-emu ${SRC_FILES})

target_include_directories(riscv-emu PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

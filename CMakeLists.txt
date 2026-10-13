# CMakeLists.txt - build configuration for the environment check.
# Builds one small C++17 executable, env_check, to confirm that CMake and
# the C++ compiler work on this machine.
cmake_minimum_required(VERSION 3.16)
project(env_check CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(env_check src/env_check.cpp)
target_compile_options(env_check PRIVATE -Wall -Wextra)

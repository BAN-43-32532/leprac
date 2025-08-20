cmake_minimum_required(VERSION 4.0.0)

project(test_toml LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(test_toml main.cpp)

target_link_libraries(test_toml PRIVATE toml11::toml11)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(test_toml PRIVATE -Wno-comma)
endif()
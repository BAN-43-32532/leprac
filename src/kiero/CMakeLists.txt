cmake_minimum_required(VERSION 3.16)
project(kiero_test LANGUAGES C CXX)

#if (NOT DEFINED CMAKE_GENERATOR_PLATFORM)
#    set(CMAKE_GENERATOR_PLATFORM Win32 CACHE STRING "Force Win32 build" FORCE)
#endif()

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE MINHOOK_SRC CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/minhook/*.c)
set(EXAMPLE_DIR ${CMAKE_CURRENT_LIST_DIR}/examples/imgui)
file(GLOB_RECURSE IMPL_SRC CONFIGURE_DEPENDS ${EXAMPLE_DIR}/impl/*.cpp)
file(GLOB IMGUI_SRC CONFIGURE_DEPENDS ${EXAMPLE_DIR}/imgui/*.cpp)
set(IMGUI_EXAMPLE_DIR ${EXAMPLE_DIR}/imgui/examples)

add_library(kiero_test SHARED
        ${CMAKE_CURRENT_LIST_DIR}/kiero.cpp
        ${MINHOOK_SRC}
        ${EXAMPLE_DIR}/main.cpp
        ${IMPL_SRC}
        ${IMGUI_SRC}
        ${IMGUI_EXAMPLE_DIR}/imgui_impl_win32.cpp
        ${IMGUI_EXAMPLE_DIR}/imgui_impl_dx11.cpp
)

target_include_directories(kiero_test PRIVATE ${EXAMPLE_DIR}/imgui)

target_compile_options(kiero_test PRIVATE /utf-8)

add_executable(hook main.cpp)
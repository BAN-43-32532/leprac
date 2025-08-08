cmake_minimum_required(VERSION 3.16)
project(dinput8_injector LANGUAGES CXX)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MACHINE:X86")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MACHINE:X86")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(hook_dll SHARED hook_dll.cpp)
target_link_libraries(hook_dll PRIVATE minhook)
target_compile_definitions(hook_dll PRIVATE _CRT_SECURE_NO_WARNINGS)

set_target_properties(hook_dll PROPERTIES
    OUTPUT_NAME "hook_dll"
    SUFFIX ".dll"
    LINK_FLAGS "/DYNAMICBASE:NO"  # 禁用ASLR以便注入
)

add_executable(injector injector.cpp)
target_compile_definitions(injector PRIVATE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(hook_dll PRIVATE dinput8 dxguid ole32)

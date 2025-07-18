cmake_minimum_required(VERSION 3.16)
project(leprac LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(leprac WIN32 ${SRC})

set(EXT_DIR ${CMAKE_SOURCE_DIR}/external)

target_include_directories(leprac PRIVATE ${EXT_DIR})

target_link_libraries(leprac PRIVATE ntdll)

# SDL ##########################################################################
add_subdirectory(${EXT_DIR}/SDL)
target_link_libraries(leprac PRIVATE SDL3::SDL3)
################################################################################

# imgui ########################################################################
set(IMGUI_DIR ${EXT_DIR}/imgui)
file(GLOB IMGUI_SRC CONFIGURE_DEPENDS ${IMGUI_DIR}/*.cpp)
target_include_directories(leprac PRIVATE ${IMGUI_DIR})
target_sources(leprac PRIVATE
        ${IMGUI_SRC}
        ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)
################################################################################

# kiero ########################################################################
file(GLOB_RECURSE KIERO_MINHOOK_SRC CONFIGURE_DEPENDS ${EXT_DIR}/kiero/minhook/*.cpp)
target_sources(leprac PRIVATE ${EXT_DIR}/kiero/kiero.cpp ${KIERO_MINHOOK_SRC})
################################################################################

# libmem #######################################################################
include(FetchContent)
set(LIBMEM_DOWNLOAD_VERSION "5.0.2" CACHE STRING "libmem version to download")
FetchContent_Declare(
        libmem-config
        URL "https://raw.githubusercontent.com/rdbo/libmem/config-v1/libmem-config.cmake"
        DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(libmem-config)
list(APPEND CMAKE_PREFIX_PATH "${libmem-config_SOURCE_DIR}")
find_package(libmem CONFIG REQUIRED)
target_link_libraries(leprac PRIVATE libmem::libmem)
################################################################################
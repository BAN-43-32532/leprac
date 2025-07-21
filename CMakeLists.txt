cmake_minimum_required(VERSION 3.16)
project(leprac LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "$<1:${CMAKE_BINARY_DIR}/bin>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "$<1:${CMAKE_BINARY_DIR}/bin>")
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "$<1:${CMAKE_BINARY_DIR}/bin>")

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(leprac WIN32 ${SRC})

set(EXT_DIR ${CMAKE_SOURCE_DIR}/external)

target_include_directories(leprac PRIVATE ${EXT_DIR})

target_link_libraries(leprac PRIVATE ntdll)

target_compile_options(leprac PRIVATE /utf-8)

include(FetchContent)

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
#file(GLOB_RECURSE KIERO_MINHOOK_SRC CONFIGURE_DEPENDS ${EXT_DIR}/kiero/minhook/*.cpp)
#target_sources(leprac PRIVATE ${EXT_DIR}/kiero/kiero.cpp ${KIERO_MINHOOK_SRC})
################################################################################

# libmem #######################################################################
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

# tomlplusplus #################################################################
add_subdirectory(${EXT_DIR}/tomlplusplus)
target_link_libraries(leprac PRIVATE tomlplusplus::tomlplusplus)
################################################################################

# embed ########################################################################
add_subdirectory(external/embed)
set(B_PRODUCTION_MODE ON CACHE BOOL "Enable production mode" FORCE)

file(GLOB_RECURSE ASSET_FILES RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/asset/*)
foreach(FILE IN LISTS ASSET_FILES)
    b_embed(leprac ${FILE})
endforeach()
################################################################################

add_subdirectory(external/kiero)
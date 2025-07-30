cmake_minimum_required(VERSION 3.28)
project(leprac LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

file(GLOB SRC CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(leprac WIN32 ${SRC})

file(GLOB MODULE CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/module/*.cppm)
target_sources(leprac PUBLIC FILE_SET CXX_MODULES FILES ${MODULE})

set(EXT_DIR ${CMAKE_SOURCE_DIR}/external)

target_include_directories(leprac PRIVATE ${EXT_DIR})
set(WINDLL d3d11 dxgi dxguid ntdll)
target_link_libraries(leprac PRIVATE ${WINDLL})
target_compile_options(leprac PRIVATE /utf-8)

include(FetchContent)

macro(external)
    add_curl()
    add_embed()
    add_imgui()
    add_json()
    add_libmem()
    add_magic_enum()
    add_minhook()
    add_toml11()
endmacro()

function(add_curl)
    set(ENABLE_CURL_MANUAL OFF CACHE BOOL "" FORCE)
    set(BUILD_LIBCURL_DOCS OFF CACHE BOOL "" FORCE)
    set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
    set(CURL_DISABLE_LDAP ON CACHE BOOL "" FORCE)
    set(HTTP_ONLY ON CACHE BOOL "" FORCE)
    set(CURL_BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(CURL_ZLIB OFF CACHE BOOL "" FORCE)
    set(CURL_BROTLI OFF CACHE BOOL "" FORCE)
    set(CURL_ZSTD OFF CACHE BOOL "" FORCE)
    set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(${EXT_DIR}/curl)
    target_link_libraries(leprac PRIVATE libcurl)
endfunction()

function(add_embed)
    add_subdirectory(external/embed)
    set(B_PRODUCTION_MODE ON CACHE BOOL "" FORCE)

    file(GLOB_RECURSE ASSET_FILES RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/asset/*)
    foreach (FILE IN LISTS ASSET_FILES)
        b_embed(leprac ${FILE})
    endforeach ()
endfunction()

function(add_imgui)
    file(GLOB IMGUI_SRC CONFIGURE_DEPENDS ${EXT_DIR}/imgui/*.cpp)
    set(IMGUI_IMPL_SRC
            ${EXT_DIR}/imgui/backends/imgui_impl_win32.cpp
            ${EXT_DIR}/imgui/backends/imgui_impl_dx11.cpp
    )
    target_include_directories(leprac PRIVATE ${EXT_DIR}/imgui)
    target_sources(leprac PRIVATE ${IMGUI_SRC} ${IMGUI_IMPL_SRC})
endfunction()

function(add_json)
#    set(NLOHMANN_JSON_BUILD_MODULES ON CACHE BOOL "" FORCE)
    add_subdirectory(${EXT_DIR}/json)
    target_link_libraries(leprac PRIVATE nlohmann_json::nlohmann_json)
#    target_link_libraries(leprac PRIVATE nlohmann_json_modules)

endfunction()

function(add_libmem)
    # It doesn't work with add_subdirectory(libmem); neither with version > 5.0.2
    set(LIBMEM_DOWNLOAD_VERSION "5.0.2" CACHE STRING "" FORCE)
    FetchContent_Declare(
            libmem-config
            URL "https://raw.githubusercontent.com/rdbo/libmem/config-v1/libmem-config.cmake"
            DOWNLOAD_NO_EXTRACT TRUE
    )
    FetchContent_MakeAvailable(libmem-config)
    list(APPEND CMAKE_PREFIX_PATH "${libmem-config_SOURCE_DIR}")
    find_package(libmem CONFIG REQUIRED)
    target_link_libraries(leprac PRIVATE libmem::libmem)
endfunction()

function(add_magic_enum)
    target_include_directories(leprac PRIVATE ${EXT_DIR}/magic_enum/include)
#    target_sources(leprac PUBLIC FILE_SET CXX_MODULES FILES
#            ${EXT_DIR}/magic_enum/module/magic_enum.cppm)
endfunction()

function(add_minhook)
    add_subdirectory(${EXT_DIR}/minhook)
    target_link_libraries(leprac PRIVATE minhook)
endfunction()

function(add_toml11)
    add_subdirectory(${EXT_DIR}/toml11)
    target_link_libraries(leprac PRIVATE toml11::toml11)
endfunction()

external()
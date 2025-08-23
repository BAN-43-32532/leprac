cmake_minimum_required(VERSION 3.28)
project(leprac LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_executable(leprac WIN32)

file(GLOB MODULE CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/module/*.cpp)
target_sources(leprac PRIVATE FILE_SET CXX_MODULES FILES ${MODULE})

file(GLOB SRC CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
target_sources(leprac PRIVATE ${SRC})

target_sources(leprac PRIVATE ${CMAKE_SOURCE_DIR}/asset/icon/resource.rc)


set(EXT_DIR ${CMAKE_SOURCE_DIR}/external)
target_include_directories(leprac PRIVATE ${EXT_DIR} ${CMAKE_SOURCE_DIR}/module)
target_include_directories(leprac PRIVATE asset/style)

# Required by launcher & overlay using ImGui DX11 & WIN32 backends. Len'en uses DX11.
target_link_libraries(leprac PRIVATE d3d11 dxgi dxguid ntdll)

# Seems ok if don't define them
target_compile_definitions(leprac PRIVATE UNICODE _UNICODE NOMINMAX)

include(FetchContent)

# I only build and test leprac with MSVC
if (MSVC)
    target_compile_options(leprac PRIVATE
        /utf-8
        $<$<CONFIG:Release>:/Ox>
        $<$<CONFIG:Release>:/Gy>
        $<$<CONFIG:Release>:/Gw>
        $<$<CONFIG:Release>:/GL>
    )
    target_link_options(leprac PRIVATE
        $<$<CONFIG:Release>:/OPT:REF>
        $<$<CONFIG:Release>:/LTCG>
    )
endif ()

macro(external)
    # add_aho_corasick: For custom style parsing (& maybe pattern match); use my modification.
    add_curl() # Only for auto-update. May replace in the future
    add_embed() # Asset data & literal embedding
    add_imgui() # Launcher & overlay GUI
    add_json() # Only for auto-update. May replace (maybe with rapidjson) in the future
    add_libmem() # Memory manipulation
    add_magic_enum() # Enum reflection
    add_minhook() # Overlay injection
    add_spdlog() # File / console logging
    # portable-file-dialogs: File dialog
    add_toml11() # Config & asset parsing
endmacro()

add_subdirectory(hook)
add_subdirectory(test)

function(add_curl)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
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
    file(GLOB_RECURSE ASSET_FILES RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/asset/*.toml)
    foreach (FILE IN LISTS ASSET_FILES)
        b_embed(leprac ${FILE})
    endforeach ()
endfunction()

function(add_imgui)
    file(GLOB IMGUI_SRC CONFIGURE_DEPENDS ${EXT_DIR}/imgui/*.cpp)
    target_include_directories(leprac PRIVATE ${EXT_DIR}/imgui)
    target_sources(leprac PRIVATE ${IMGUI_SRC}
        ${EXT_DIR}/imgui/backends/imgui_impl_win32.cpp
        ${EXT_DIR}/imgui/backends/imgui_impl_dx11.cpp
    )
endfunction()

function(add_json)
    add_subdirectory(${EXT_DIR}/json)
    target_link_libraries(leprac PRIVATE nlohmann_json)
    # cpp20 module fails
    # set(NLOHMANN_JSON_BUILD_MODULES ON CACHE BOOL "" FORCE)
    # target_link_libraries(leprac PRIVATE nlohmann_json_modules)
endfunction()

function(add_libmem)
    # git clone & add_subdirectory doesn't work; neither with version > 5.0.2
    set(LIBMEM_DOWNLOAD_VERSION "5.0.2" CACHE STRING "" FORCE)
    FetchContent_Declare(
        libmem-config
        URL "https://raw.githubusercontent.com/rdbo/libmem/config-v1/libmem-config.cmake"
        DOWNLOAD_NO_EXTRACT TRUE
    )
    FetchContent_MakeAvailable(libmem-config)
    list(APPEND CMAKE_PREFIX_PATH ${libmem-config_SOURCE_DIR})
    find_package(libmem CONFIG REQUIRED)
    target_link_libraries(leprac PRIVATE libmem::libmem)
endfunction()

function(add_magic_enum)
    target_include_directories(leprac PRIVATE ${EXT_DIR}/magic_enum/include)
    target_sources(leprac PRIVATE FILE_SET CXX_MODULES FILES ${EXT_DIR}/magic_enum/module/magic_enum.cppm)
endfunction()

function(add_minhook)
    add_subdirectory(${EXT_DIR}/minhook)
    target_link_libraries(leprac PRIVATE minhook)
endfunction()

function(add_spdlog)
    add_subdirectory(${EXT_DIR}/spdlog)
    target_link_libraries(leprac PRIVATE spdlog::spdlog)
endfunction()

function(add_toml11)
    add_subdirectory(${EXT_DIR}/toml11)
    target_link_libraries(leprac PRIVATE toml11::toml11)
endfunction()

external()
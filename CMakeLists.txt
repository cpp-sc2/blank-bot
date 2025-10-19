# The MIT License (MIT)
#
# Copyright (c) 2021-2024 Alexander Kurbatov

cmake_minimum_required(VERSION 3.15)

# Skip generating proto files with Protobuf compiler and use prebuilt files
# Will still build Protobuf as libprotobuf is required for linking
option(WSL2_CROSS_COMPILE "Building on WSL2 for Windows" OFF)

# Only valid OS configuration should be in WSL2 context
# Should not work wtih WIN32, APPLE, etc
if (WSL2_CROSS_COMPILE AND UNIX AND NOT APPLE)
    set(CMAKE_TOOLCHAIN_FILE cmake/toolchain/x86-64-w64-mingw32.cmake)
elseif (WSL2_CROSS_COMPILE)
    message(FATAL_ERROR "Invalid environment.\
                        WSL2_CROSS_COMPILE=ON for WSL only")
endif ()

project(BlankBot)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

option(BUILD_FOR_LADDER "Create build for the AIArena ladder" OFF)

# Build with c++17 support, required by sc2api
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify output directories
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

if (MSVC)
    # Setup MSVC parallelized builds
    add_compile_options(/MP)

    # Use statically linked runtime
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)
endif ()

list(APPEND CMAKE_MODULE_PATH thirdparty/cmake)

# cpp-sc2 dependency
include(cpp_sc2)

# bot sources
add_subdirectory(src)

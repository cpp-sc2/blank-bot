# The MIT License (MIT)
#
# Copyright (c) 2021-2022 Alexander Kurbatov

cmake_minimum_required(VERSION 3.15)

project(BlankBot)

include(FetchContent)

option(BUILD_FOR_LADDER "Create build for the AIArena ladder" OFF)

# Build with c++14 support, required by sc2api.
set(CMAKE_CXX_STANDARD 14)

# Specify output directories.
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

if (MSVC)
    # Setup MSVC parallelized builds
    add_compile_options(/MP)

    # Use statically linked runtime
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)
endif ()

list(APPEND CMAKE_MODULE_PATH cmake)

# cpp-sc2 dependency
include(cpp_sc2)

# bot sources
add_subdirectory(src)

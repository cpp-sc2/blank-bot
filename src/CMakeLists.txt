# The MIT License (MIT)
#
# Copyright (c) 2021-2022 Alexander Kurbatov

if (BUILD_FOR_LADDER)
    ADD_DEFINITIONS(-DBUILD_FOR_LADDER)
endif ()

# All the source files for the bot.
file(GLOB_RECURSE BOT_SOURCES "*.cpp" "*.h")

include_directories(SYSTEM
    ${PROJECT_SOURCE_DIR}/contrib/cpp-sc2/include
    ${PROJECT_SOURCE_DIR}/contrib/cpp-sc2/contrib/protobuf/src
    ${PROJECT_BINARY_DIR}/contrib/cpp-sc2/generated
    "."
)

link_directories(${PROJECT_BINARY_DIR}/contrib/cpp-sc2/bin)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /EHsc")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif ()

# Create the executable.
add_executable(BlankBot ${BOT_SOURCES})
target_link_libraries(BlankBot PRIVATE cpp_sc2)

if (MINGW)
    target_link_libraries(BlankBot PRIVATE ssp)
endif ()

if (APPLE)
    target_link_libraries(BlankBot PRIVATE "-framework Carbon")
endif ()

# Linux specific.
if (UNIX AND NOT APPLE)
    target_link_libraries(BlankBot PRIVATE pthread dl)
endif ()

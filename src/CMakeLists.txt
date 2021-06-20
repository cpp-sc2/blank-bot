# The MIT License (MIT)
#
# Copyright (c) 2021 Alexander Kurbatov

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

# Create the executable.
add_executable(BlankBot ${BOT_SOURCES})
target_link_libraries(BlankBot
    sc2api sc2lib sc2utils sc2protocol civetweb libprotobuf
)

if (MINGW)
    target_link_libraries(BlankBot ssp)
endif ()

if (APPLE)
    target_link_libraries(BlankBot "-framework Carbon")
endif ()

# Linux specific.
if (UNIX AND NOT APPLE)
    target_link_libraries(BlankBot pthread dl)
endif ()

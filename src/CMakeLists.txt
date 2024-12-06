# The MIT License (MIT)
#
# Copyright (c) 2021-2024 Alexander Kurbatov

set(bot_sources
    main.cpp)

add_executable(BlankBot ${bot_sources})

if (BUILD_FOR_LADDER)
    target_compile_definitions(BlankBot PRIVATE BUILD_FOR_LADDER)
endif ()

if (MSVC)
    target_compile_options(BlankBot PRIVATE /W4 /EHsc)
else ()
    target_compile_options(BlankBot PRIVATE -Wall -Wextra -pedantic)
endif ()

target_link_libraries(BlankBot PRIVATE cpp_sc2)

if (MINGW)
    target_link_libraries(BlankBot PRIVATE ssp)
elseif (APPLE)
    target_link_libraries(BlankBot PRIVATE "-framework Carbon")
# Building on Linux
elseif (UNIX AND NOT APPLE)
    target_link_libraries(BlankBot PRIVATE pthread dl)
endif ()

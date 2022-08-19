message(STATUS "FetchContent: cpp-sc2")

# Dont build examples in the sc2api submodule.
set(BUILD_API_EXAMPLES OFF CACHE INTERNAL "" FORCE)

# Dont build tests in the sc2api submodule.
set(BUILD_API_TESTS OFF CACHE INTERNAL "" FORCE)

# Dont build sc2renderer in the sc2api submodule.
set(BUILD_SC2_RENDERER OFF CACHE INTERNAL "" FORCE)

FetchContent_Declare(
    cpp_sc2
    GIT_REPOSITORY https://github.com/cpp-sc2/cpp-sc2.git
    GIT_TAG 60befd4786d27f71f11a69796af87b7c23b5e7aa
)
FetchContent_MakeAvailable(cpp_sc2)

set_target_properties(sc2api PROPERTIES FOLDER cpp-sc2)
set_target_properties(sc2lib PROPERTIES FOLDER cpp-sc2)
set_target_properties(sc2protocol PROPERTIES FOLDER cpp-sc2)
set_target_properties(sc2utils PROPERTIES FOLDER cpp-sc2)

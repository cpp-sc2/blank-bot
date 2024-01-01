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
    GIT_TAG 5bc693e61945f99354fda448b83694421d3e3023
)
FetchContent_MakeAvailable(cpp_sc2)

set_target_properties(sc2api PROPERTIES FOLDER cpp-sc2)
set_target_properties(sc2lib PROPERTIES FOLDER cpp-sc2)
set_target_properties(sc2protocol PROPERTIES FOLDER cpp-sc2)
set_target_properties(sc2utils PROPERTIES FOLDER cpp-sc2)

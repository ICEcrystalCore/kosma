cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_TESTS "Build tests" ON)

include(cmake/LoadCMakeScripts.cmake)

load_all_cmake_scripts()

init_vcpkg_env()

project(Kosma)

init_env_vars()

declare_options()

update_config()

set_global_output_directory(Output)

scann_all_third_party_targets(3rdParty)

add_subdirectory(Source)

if (BUILD_TESTS)
    add_subdirectory(Tests)
endif ()
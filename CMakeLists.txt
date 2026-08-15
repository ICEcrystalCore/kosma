cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_TESTS "Build tests" ON)

include(cmake/LoadCMakeScripts.cmake)

load_all_cmake_scripts()

run_conan_install()

project(Kosma)

init_env_vars()

declare_options()

update_config()

set_global_output_directory(Output)

scan_all_third_party_targets(third_party)

# Setup LLDB visualizers for debug builds
setup_lldb_visualizers_global()

add_subdirectory(source)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif ()
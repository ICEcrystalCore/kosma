function(declare_options)
    option(BUILD_TESTS "Build tests" ON)
    option(BUILD_STATIC "Build static library" OFF)

    option(ENABLE_VERBOSE "VERBOSE OUTPUT" OFF)
    option(USING_DWARF_4 "Use DWARF 4 format" OFF)

endfunction()

function(update_config)
    init_config_directory()

    if (NOT IS_WIN AND USING_DWARF_4)
        add_compile_definitions(-gdwarf-4)
    endif ()

    if (ENABLE_VERBOSE)
        set(CMAKE_VERBOSE_MAKEFILE ON CACHE STRING "VERBOSE BUILD" FORCE)
    else ()
        set(CMAKE_VERBOSE_MAKEFILE OFF CACHE STRING "VERBOSE BUILD" FORCE)
    endif ()
endfunction()

function(init_config_directory)
    get_filename_component(_CONFIG_ROOT "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../Config" ABSOLUTE)
    set(CONFIG_ROOT ${_CONFIG_ROOT} CACHE PATH "Config root directory" FORCE)
endfunction()
function(init_env_vars)
    reset_env_vars()

    # get system name
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(IS_WIN TRUE CACHE BOOL "Is Windows")
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(IS_LINUX TRUE CACHE BOOL "Is Linux")
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        set(IS_MAC TRUE CACHE BOOL "Is Mac")
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Android")
        set(IS_ANDROID TRUE CACHE BOOL "Is Android")
    elseif (CMAKE_SYSTEM_NAME STREQUAL "iOS")
        set(IS_IOS TRUE CACHE BOOL "Is iOS")
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        set(IS_EMSCRIPTEN TRUE CACHE BOOL "Is Emscripten")
    endif ()

    # get compiler name
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(IS_MSVC TRUE CACHE BOOL "Is MSVC")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(IS_GNU TRUE CACHE BOOL "Is GNU")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(IS_CLANG TRUE CACHE BOOL "Is Clang")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        set(IS_APPLECLANG TRUE CACHE BOOL "Is AppleClang")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
        set(IS_EMSCRIPTEN TRUE CACHE BOOL "Is Emscripten")
    endif ()

    if (CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
        set(IS_MSVC_LIKE TRUE CACHE BOOL "Is Emulator")
    endif ()


    # get compiler version
    set(COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION} CACHE STRING "Compiler Version")

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(IS_64BIT TRUE CACHE BOOL "Is 64bit")
    elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(IS_32BIT TRUE CACHE BOOL "Is 32bit")
    endif ()

    # get build type
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(IS_DEBUG TRUE CACHE BOOL "Is Debug")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(IS_RELEASE TRUE CACHE BOOL "Is Release")
    endif ()

endfunction()

function(reset_env_vars)
    # get global variants name start with is_
    get_cmake_property(GLOBAL_VAR_LIST CACHE_VARIABLES)
    foreach (GLOBAL_VAR ${GLOBAL_VAR_LIST})
        if (GLOBAL_VAR MATCHES "^is_")
            unset(${GLOBAL_VAR} CACHE)
        endif ()
    endforeach ()
endfunction()
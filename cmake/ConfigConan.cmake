function(run_conan_install)
    # 1. 查找 conan
    find_program(CONAN_CMD conan)
    if (NOT CONAN_CMD)
        message(FATAL_ERROR "Conan not found! Please install Conan (e.g. pip install conan)")
    endif ()
    message(STATUS "Conan found: ${CONAN_CMD}")

    set(PKG_MGR "conan" CACHE STRING "Package manager to use")

    # 2. 确保 default profile 存在，否则执行 profile detect
    execute_process(
            COMMAND ${CONAN_CMD} profile path default
            RESULT_VARIABLE PROFILE_RESULT
            OUTPUT_QUIET ERROR_QUIET
    )

    if (NOT PROFILE_RESULT EQUAL 0)
        message(STATUS "Conan default profile missing, running 'conan profile detect'...")
        execute_process(
                COMMAND ${CONAN_CMD} profile detect
                RESULT_VARIABLE DETECT_RESULT
        )
        if (NOT DETECT_RESULT EQUAL 0)
            message(FATAL_ERROR "Conan profile detect failed!")
        endif ()
    else ()
        message(STATUS "Conan default profile exists, skip detect.")
    endif ()


    set(CONAN_BUILD_DIR ${CMAKE_BINARY_DIR}/conan_build)

    execute_process(
            COMMAND ${CONAN_CMD} install ${CMAKE_SOURCE_DIR}
            --output-folder=${CONAN_BUILD_DIR}
            --build=missing
            -s compiler.cppstd=20
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            RESULT_VARIABLE CONAN_INSTALL_RESULT
    )

    if (NOT CONAN_INSTALL_RESULT EQUAL 0)
        message(FATAL_ERROR "Conan install failed!")
    endif ()

    set(CMAKE_MODULE_PATH ${CONAN_BUILD_DIR} ${CMAKE_MODULE_PATH} CACHE PATH "CMake module path updated by Conan" FORCE)
    set(CMAKE_PREFIX_PATH ${CONAN_BUILD_DIR} ${CMAKE_PREFIX_PATH} CACHE PATH "CMake prefix path updated by Conan" FORCE)
endfunction()
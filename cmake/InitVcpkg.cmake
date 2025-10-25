function(init_vcpkg_env)
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})

    if (NOT EXISTS ${VCPKG_ROOT})
        message(FATAL_ERROR "vcpkg not found")
    endif ()

    set(VCPKG_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_TOOLCHAIN_FILE}" CACHE STRING "toolchain file" FORCE)

    set(PKG_MGR "vcpkg" CACHE STRING "Package manager to use")
endfunction()
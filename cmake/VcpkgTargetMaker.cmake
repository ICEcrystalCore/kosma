function(make_vcpkg_target TARGET_NAME)
    # Ensure target_name is not already created
    if (NOT TARGET ${TARGET_NAME})
        add_library(${TARGET_NAME} INTERFACE)
    endif ()
endfunction()

function(add_vcpkg_pack_for TARGET_NAME PACK_NAME)
    if (ARGC GREATER 2)
        set(COMPONENTS "")
        # find all components name from the rest of the arguments
        foreach (COMPONENT IN LISTS ARGN)
            list(APPEND COMPONENTS ${COMPONENT})
        endforeach ()
        find_package(${PACK_NAME} COMPONENTS ${COMPONENTS} REQUIRED)
    else ()
        find_package(${PACK_NAME} REQUIRED)
    endif ()
endfunction()

function(add_vcpkg_libraries_for TARGET_NAME)
    if (NOT TARGET ${TARGET_NAME})
        return()
    endif ()

    foreach (LIBRARIES IN LISTS ARGN)
        target_link_libraries(${TARGET_NAME} INTERFACE ${LIBRARIES})
    endforeach ()
endfunction()
# load all cmake scripts in same folder
function(load_all_cmake_scripts)
    # get current function name
    set(SELF_NAME ${CMAKE_CURRENT_FUNCTION_LIST_FILE})
    # load all cmake scripts in same folder
    file(GLOB_RECURSE CMAKE_SCRIPT_LIST "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/*.cmake")
    # load all cmake scripts
    foreach (CMAKE_SCRIPT ${CMAKE_SCRIPT_LIST})
        # skip this file
        if (${CMAKE_SCRIPT} STREQUAL SELF_NAME)
            continue()
        endif ()
        include(${CMAKE_SCRIPT})
    endforeach ()
endfunction()
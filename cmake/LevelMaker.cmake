function(register_levels)
    set(_LEVELS)
    foreach (_LEVEL ${ARGN})
        list(APPEND _LEVELS ${_LEVEL})
    endforeach ()
    set(LEVELS ${_LEVELS} CACHE INTERNAL "levels in order")
    set(UNSETTED_LEVELS ${LEVELS} CACHE INTERNAL "unsetted levels")
endfunction()

function(setup_level LEVEL_NAME DIR)
    reset_current_level()

    # check UNSETTED_LEVELS if contains LEVEL_NAME
    is_contain("${UNSETTED_LEVELS}" ${LEVEL_NAME} _is_unsetted)

    if (NOT _is_unsetted)
        is_contain("${LEVELS}" ${LEVEL_NAME} _is_registered)
        if (_is_registered)
            message(FATAL_ERROR "level ${LEVEL_NAME} is already set")
        else ()
            message(FATAL_ERROR "level ${LEVEL_NAME} is not registered")
        endif ()

    endif ()

    set_current_level(${LEVEL_NAME})
    add_subdirectory(${DIR})
endfunction()

function(is_contain LIST ITEM RESULT)
    list(FIND LIST ${ITEM} _INDEX)
    if (${_INDEX} EQUAL -1)
        set(${RESULT} FALSE PARENT_SCOPE)
    else ()
        set(${RESULT} TRUE PARENT_SCOPE)
    endif ()
endfunction()

function(reset_current_level)
    unset(CURRENT_LEVEL CACHE)
endfunction()

function(set_current_level LEVEL_NAME)
    set(CURRENT_LEVEL ${LEVEL_NAME} CACHE INTERNAL "current level")
endfunction()
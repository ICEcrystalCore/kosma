function(make_module MODULE_NAME)
    _find_folder(Public PUBLIC_DIR)

    _find_folder(Private PRIVATE_DIR)

    if (NOT PUBLIC_DIR)
        message(FATAL_ERROR "Public folder not found")
    endif ()

    if (NOT PRIVATE_DIR)
        message(NOTICE "[NOTICE] No source file found in module '${MODULE_NAME}'")
        _add_interface_library(${MODULE_NAME} ${PUBLIC_DIR})
        return()
    endif ()

    _find_source_files_under(${PRIVATE_DIR} SOURCE_LIST)

    _make_library_from_sources(${MODULE_NAME} "${SOURCE_LIST}")

    _config_compile_definitions(${MODULE_NAME})

    _gen_api_header(${MODULE_NAME})

    target_include_directories(${MODULE_NAME} PUBLIC ${PUBLIC_DIR})

    target_include_directories(${MODULE_NAME} PRIVATE ${PRIVATE_DIR})
endfunction()

function(make_module_under_level MODULE_NAME)
    make_module(${MODULE_NAME})
endfunction()

function(link_public_target MODULE_NAME TARGET_NAME)
    # link target to module
    target_link_libraries(${MODULE_NAME} PUBLIC ${TARGET_NAME})
endfunction()

function(link_private_target MODULE_NAME TARGET_NAME)
    target_link_libraries(${MODULE_NAME} PRIVATE ${TARGET_NAME})
endfunction()

# ---- Private Functions ----

function(_add_interface_library MODULE_NAME DIR)
    add_library(${MODULE_NAME} INTERFACE ${DIR})
    set_target_properties(${MODULE_NAME} PROPERTIES HEADER_ONLY ON)
endfunction()

function(_find_folder FOLDER_NAME FOLDER_PATH)
    file(GLOB _FOLDER_LIST ${FOLDER_NAME})
    set(${FOLDER_PATH} ${_FOLDER_LIST} PARENT_SCOPE)
endfunction()

function(_find_source_files_under DIR SOURCE_LIST)
    file(GLOB_RECURSE _SOURCE_LIST ${DIR}/*.cpp ${DIR}/*.c ${DIR}/*.cc ${DIR}/*.cxx)
    set(${SOURCE_LIST} ${_SOURCE_LIST} PARENT_SCOPE)
endfunction()

function(_make_objects_for OBJECT_TARGET SOURCE_LIST)
    add_library(${OBJECT_TARGET} OBJECT ${SOURCE_LIST})
    if (NOT IS_WIN AND NOT BUILD_STATIC)
        set_target_properties(${OBJECT_TARGET} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif ()
    # get other arguments
    list(LENGTH ARGN ARG_COUNT)
    foreach (EXTRA_INCLUDES_DIR ${ARGN})
        target_include_directories(${OBJECT_TARGET} PRIVATE ${EXTRA_INCLUDES_DIR})
    endforeach ()
endfunction()

function(_make_library_from_objects MODULE_NAME OBJECT_TARGET)
    if (BUILD_STATIC)
        add_library(${MODULE_NAME} STATIC $<TARGET_OBJECTS:${OBJECT_TARGET}>)
    else ()
        add_library(${MODULE_NAME} SHARED $<TARGET_OBJECTS:${OBJECT_TARGET}>)
    endif ()
endfunction()

function(_make_library_from_sources MODULE_NAME SOURCE_FILES)
    if (BUILD_STATIC)
        add_library(${MODULE_NAME} STATIC ${SOURCE_FILES})
    else ()
        add_library(${MODULE_NAME} SHARED ${SOURCE_FILES})
    endif ()
endfunction()

function(_config_compile_definitions MODULE_NAME)
    if (IS_WIN AND (IS_MSVC OR IS_MSVC_LIKE))
        target_compile_definitions(${MODULE_NAME} PRIVATE BUILDING_DLL)
    endif ()

    _convert_pascal_to_upper_snake(${MODULE_NAME} SNACK_MODULE_NAME)

    target_compile_definitions(${MODULE_NAME} PRIVATE BUILDING_${SNACK_MODULE_NAME})
endfunction()

function(_convert_pascal_to_upper_snake ORIGIN RESULT)
    # convert pascal style naming to upper snake style naming
    string(REGEX REPLACE "([A-Z])" "_\\1" _ADD_UNDERLINE ${ORIGIN})
    string(REGEX REPLACE "^_" "" _REMOVE_FIRST_UNDERLINE ${_ADD_UNDERLINE})
    string(TOUPPER ${_REMOVE_FIRST_UNDERLINE} _RESULT)
    set(${RESULT} ${_RESULT} PARENT_SCOPE)
endfunction()

function(_gen_api_header MODULE_NAME)
    _convert_pascal_to_upper_snake(${MODULE_NAME} SNACK_MODULE_NAME)

    get_mock_folder_for(${MODULE_NAME} MOCK_FOLDER)

    set(_MOCK_API_NAME "${MODULE_NAME}API.h")

    configure_file(
            ${CMAKE_SOURCE_DIR}/BuildConfigs/APITemplate.h.in
            ${MOCK_FOLDER}/${_MOCK_API_NAME}
            @ONLY
    )

    target_include_directories(${MODULE_NAME} PUBLIC ${MOCK_FOLDER})
endfunction()
function(make_shared_imported_target TARGET_NAME)
    add_library(${TARGET_NAME} SHARED IMPORTED GLOBAL)
    target_include_directories(${TARGET_NAME} INTERFACE ${3RD_PARTY_INC_DIR})
endfunction()

function(make_static_imported_target TARGET_NAME)
    add_library(${TARGET_NAME} STATIC IMPORTED GLOBAL)
    target_include_directories(${TARGET_NAME} INTERFACE ${3RD_PARTY_INC_DIR})
endfunction()

function(make_interface_imported_target TARGET_NAME)
    add_library(${TARGET_NAME} INTERFACE IMPORTED GLOBAL)
    target_include_directories(${TARGET_NAME} INTERFACE ${3RD_PARTY_INC_DIR})
endfunction()

function(add_imported_target_library_file TARGET_NAME LIBRARY_FILE)
    is_shared_target(${TARGET_NAME} IS_SHARED)
    get_library_dir(${IS_SHARED} LIBRARY_DIR)
    set_property(TARGET ${TARGET_NAME} PROPERTY IMPORTED_LOCATION ${LIBRARY_DIR}/${LIBRARY_FILE})
endfunction()

function(add_import_target_dll_import_library_file TARGET_NAME LIBRARY_FILE)
    get_library_dir(FALSE LIBRARY_DIR)
    set_property(TARGET ${TARGET_NAME} PROPERTY IMPORTED_IMPLIB ${LIBRARY_DIR}/${LIBRARY_FILE})
endfunction()

function(is_imported_target TARGET_NAME OUT_IS_IMPORTED)
    get_target_property(_TARGET_TYPE ${TARGET_NAME} TYPE)
    if (_TARGET_TYPE STREQUAL "IMPORTED")
        set(${OUT_IS_IMPORTED} TRUE PARENT_SCOPE)
    else ()
        set(${OUT_IS_IMPORTED} FALSE PARENT_SCOPE)
    endif ()
endfunction()

#--- Private ---

function(is_shared_target TARGET_NAME OUT_IS_SHARED)
    get_target_property(_TARGET_TYPE ${TARGET_NAME} TYPE)
    if (_TARGET_TYPE STREQUAL "SHARED_LIBRARY")
        set(${OUT_IS_SHARED} TRUE PARENT_SCOPE)
    else ()
        set(${OUT_IS_SHARED} FALSE PARENT_SCOPE)
    endif ()
endfunction()

function(get_library_dir IS_SHARED OUT_LIBRARY_DIR)
    if (IS_SHARED)
        set(${OUT_LIBRARY_DIR} ${3RD_PARTY_BIN_DIR} PARENT_SCOPE)
    else ()
        set(${OUT_LIBRARY_DIR} ${3RD_PARTY_LIB_DIR} PARENT_SCOPE)
    endif ()
endfunction()


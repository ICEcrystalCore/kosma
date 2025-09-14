function(make_executable TARGET_NAME FOLDER_NAME)
    _find_source_files_under(${FOLDER_NAME} _SOURCE_LIST)

    add_executable(${TARGET_NAME} ${_SOURCE_LIST})
endfunction()
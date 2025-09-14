function(scann_all_third_party_targets DIR)
    # find all CMakeLists.txt files under the specified directory and its subdirectories
    file(GLOB_RECURSE CMAKE_FILES ${DIR}/*/CMakeLists.txt)

    foreach (CMAKE_FILE IN LISTS CMAKE_FILES)
        # get the directory of each CMakeLists.txt file
        get_filename_component(SUBDIR_DIR ${CMAKE_FILE} DIRECTORY)
        # add the subdirectory to the build
        add_subdirectory(${SUBDIR_DIR})
    endforeach ()
endfunction()

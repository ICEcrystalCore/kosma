function(init_unit_tests)
    _make_all_in_one_test()
    scann_all_third_party_targets(.)
endfunction()

function(make_simple_test TEST_NAME)
    _find_source_files_under(. SOURCE_LIST)
    _make_test_executable(${TEST_NAME} "${SOURCE_LIST}")
    _add_to_all_tests(${SOURCE_LIST})
endfunction()

function(link_test_module TEST_NAME TARGET_NAME)
    _get_all_tests_target_name(_ALL_TESTS)
    target_link_libraries(${TEST_NAME} PRIVATE ${TARGET_NAME})
    target_link_libraries(${_ALL_TESTS} PRIVATE ${TARGET_NAME})
endfunction()

# --- PRIVATE ---

function(_make_all_in_one_test)
    _find_source_files_under(. SOURCE_LIST)
    _get_all_tests_target_name(_ALL_TESTS)
    _make_test_executable(${_ALL_TESTS} "${SOURCE_LIST}")
endfunction()

function(_make_test_executable TEST_NAME SOURCE_LIST)
    add_executable(${TEST_NAME} ${SOURCE_LIST})
    target_link_libraries(${TEST_NAME} PRIVATE KsGTest)
endfunction()

function(_add_to_all_tests SOURCE_LIST)
    _get_all_tests_target_name(_ALL_TESTS)
    if (TARGET ${_ALL_TESTS})
        target_sources(${_ALL_TESTS} PRIVATE ${SOURCE_LIST})
    endif ()
endfunction()

function(_get_all_tests_target_name TARGET_NAME)
    set(${TARGET_NAME} All_Tests PARENT_SCOPE)
endfunction()

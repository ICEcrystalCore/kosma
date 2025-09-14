function(init_third_party_dirs)
    set_third_party_roots(${CMAKE_SOURCE_DIR}/External)
    set_third_party_dirs()
endfunction()

# --- Private ---

function(_set_third_party_roots EXTERNAL_ROOT)
    set(3RD_PARTY_INC_ROOT ${EXTERNAL_ROOT}/include CACHE PATH "3rd party include root directory")
    set(3RD_PARTY_BIN_ROOT ${EXTERNAL_ROOT}/bin CACHE PATH "3rd party binary root directory")
    set(3RD_PARTY_LIB_ROOT ${EXTERNAL_ROOT}/lib CACHE PATH "3rd party library root directory")
endfunction()

function(_set_third_party_dirs)
    get_env_folder_name(ENV_FOLDER_NAME)
    set(3RD_PARTY_INC_DIR ${3RD_PARTY_INC_ROOT} CACHE PATH "3rd party include directory")

    set(3RD_PARTY_BIN_DIR ${3RD_PARTY_BIN_ROOT}/${ENV_FOLDER_NAME} CACHE PATH "3rd party binary directory")
    set(3RD_PARTY_LIB_DIR ${3RD_PARTY_LIB_ROOT}/${ENV_FOLDER_NAME} CACHE PATH "3rd party library directory")
endfunction()

function(_get_env_folder_name ENV_FOLDER_NAME)
    if (IS_WIN)
        set(${ENV_FOLDER_NAME} "windows" PARENT_SCOPE)
    elseif (IS_LINUX)
        set(${ENV_FOLDER_NAME} "linux" PARENT_SCOPE)
    endif ()
endfunction()
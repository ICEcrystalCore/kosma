function(add_signature TARGET_NAME)
    if (NOT IS_MAC)
        return()
    endif ()

    if (NOT ENABLE_CODESIGN)
        return()
    endif ()


    if (NOT TARGET ${TARGET_NAME})
        message(FATAL_ERROR "Target '${TARGET_NAME}' does not exist.")
    endif ()

    gen_apple_codesign_id_param(CERTIFICATE_NAME)

    get_ENTITLEMENTS_FILE(ENTITLEMENTS_FILE)

    if (CERTIFICATE_NAME STREQUAL "")
        return()
    endif ()

    if (ENTITLEMENTS_FILE STREQUAL "")
        return()
    endif ()

    add_temp_sign(${TARGET_NAME} ${ENTITLEMENTS_FILE})
endfunction()

function(add_temp_sign TARGET_NAME ENTITLEMENTS_FILE)
    if (ENTITLEMENTS_FILE STREQUAL "")
        return()
    endif ()

    # 生成签名命令
    add_custom_command(
            TARGET ${TARGET_NAME}
            POST_BUILD
            COMMAND codesign -s - -v
            --entitlements "${ENTITLEMENTS_FILE}"
            --force
            $<TARGET_FILE:${TARGET_NAME}>
            COMMENT "Signing with temp certificate '${CERTIFICATE_NAME}'"
    )
endfunction()

function(gen_apple_codesign_id_param ID_PARAM)
    if (NOT IS_MAC)
        set(${ID_PARAM} "" PARENT_SCOPE)
        return()
    endif ()

    if (NOT ENABLE_CODESIGN)
        set(${ID_PARAM} "" PARENT_SCOPE)
        return()
    endif ()

    message(STATUS "dev id ${APPLE_ID} ${APPLE_TEAM_ID}")

    if (APPLE_ID STREQUAL "")
        set(${ID_PARAM} "" PARENT_SCOPE)
        return()
    endif ()

    if (APPLE_TEAM_ID STREQUAL "")
        set(${ID_PARAM} "" PARENT_SCOPE)
        return()
    endif ()

    set(${ID_PARAM} "\"Apple Development: ${APPLE_ID} (${APPLE_TEAM_ID})\"" PARENT_SCOPE)
endfunction()

function(get_ENTITLEMENTS_FILE FILE_PATH)
    set(_FILE_PATH ${CONFIG_ROOT}/DevSign.plist)
    if (EXISTS ${_FILE_PATH})
        set(${FILE_PATH} ${_FILE_PATH} PARENT_SCOPE)
    else ()
        message(FATAL_ERROR "Entitlements file not found: ${FILE_PATH}")
    endif ()
endfunction()
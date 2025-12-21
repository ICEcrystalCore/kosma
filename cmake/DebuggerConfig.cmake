# DebuggerConfig.cmake
# Functions for configuring debugger visualizers

# setup_lldb_visualizers
# Setup LLDB visualizers for a target.
# This function configures LLDB to automatically load visualizer scripts
# when debugging the specified target.
# Args:
#   TARGET_NAME: The name of the target to configure
function(setup_lldb_visualizers TARGET_NAME)
    # Always setup visualizers, they can be useful in any build type
    # The user can manually load them if needed
    
    # Get the debugger scripts directory
    get_filename_component(DEBUGGER_SCRIPTS_DIR 
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../Configs/Debugger" 
        ABSOLUTE
    )
    
    # Get the visualizer script path
    set(VISUALIZER_SCRIPT "${DEBUGGER_SCRIPTS_DIR}/KSString.py")
    
    if(NOT EXISTS "${VISUALIZER_SCRIPT}")
        message(WARNING "Visualizer script not found: ${VISUALIZER_SCRIPT}")
        return()
    endif()
    
    # Get the output directory where the executable will be located
    get_target_property(OUTPUT_DIR ${TARGET_NAME} RUNTIME_OUTPUT_DIRECTORY)
    if(NOT OUTPUT_DIR)
        set(OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endif()
    if(NOT OUTPUT_DIR)
        set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/Output)
    endif()
    
    # Create .lldbinit file in the executable's output directory
    # This ensures LLDB can find it when debugging from that directory
    set(LLDBINIT_FILE "${OUTPUT_DIR}/.lldbinit")
    
    # Use absolute path for the script to avoid path issues
    get_filename_component(VISUALIZER_SCRIPT_ABS "${VISUALIZER_SCRIPT}" ABSOLUTE)
    
    file(WRITE "${LLDBINIT_FILE}" 
        "# Auto-generated LLDB init file for ${TARGET_NAME}\n"
        "# This file is automatically loaded when LLDB starts in this directory\n\n"
        "# Load KSString visualizer\n"
        "command script import \"${VISUALIZER_SCRIPT_ABS}\"\n"
    )
    
    # Also create in build directory for convenience
    set(LLDBINIT_FILE_BUILD "${CMAKE_CURRENT_BINARY_DIR}/.lldbinit")
    file(WRITE "${LLDBINIT_FILE_BUILD}"
        "# Auto-generated LLDB init file for ${TARGET_NAME}\n"
        "# This file is automatically loaded when LLDB starts in this directory\n\n"
        "# Load KSString visualizer\n"
        "command script import \"${VISUALIZER_SCRIPT_ABS}\"\n"
    )
    
    # For Xcode, set the LLDB init commands
    if(CMAKE_GENERATOR STREQUAL "Xcode")
        # Xcode uses a different mechanism - we can set it via scheme or use a custom command
        # For now, we'll create the file and document it
        set_target_properties(${TARGET_NAME} PROPERTIES
            XCODE_ATTRIBUTE_DEBUG_INIT_COMMANDS "command script import \\\"${VISUALIZER_SCRIPT_ABS}\\\""
        )
    endif()
    
    message(STATUS "LLDB visualizers configured for ${TARGET_NAME}")
    message(STATUS "  Visualizer script: ${VISUALIZER_SCRIPT_ABS}")
    message(STATUS "  LLDB init file (output dir): ${LLDBINIT_FILE}")
    message(STATUS "  LLDB init file (build dir): ${LLDBINIT_FILE_BUILD}")
    message(STATUS "  To use manually: (lldb) command script import \"${VISUALIZER_SCRIPT_ABS}\"")
endfunction()


# setup_lldb_visualizers_global
# Setup LLDB visualizers globally for all targets.
# This function creates a global .lldbinit file that will be automatically
# loaded by LLDB when it starts.
function(setup_lldb_visualizers_global)
    # Get the debugger scripts directory
    get_filename_component(DEBUGGER_SCRIPTS_DIR 
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../Configs/Debugger" 
        ABSOLUTE
    )
    
    # Get the visualizer script path
    set(VISUALIZER_SCRIPT "${DEBUGGER_SCRIPTS_DIR}/KSString.py")
    
    if(NOT EXISTS "${VISUALIZER_SCRIPT}")
        message(WARNING "Visualizer script not found: ${VISUALIZER_SCRIPT}")
        return()
    endif()
    
    # Use absolute path for the script
    get_filename_component(VISUALIZER_SCRIPT_ABS "${VISUALIZER_SCRIPT}" ABSOLUTE)
    
    # Create a global lldbinit file in the build directory
    set(GLOBAL_LLDBINIT "${CMAKE_BINARY_DIR}/.lldbinit")
    
    file(WRITE "${GLOBAL_LLDBINIT}"
        "# Auto-generated LLDB init file for Kosma project\n"
        "# This file is automatically loaded when LLDB starts in this directory\n\n"
        "# Load KSString visualizer\n"
        "command script import \"${VISUALIZER_SCRIPT_ABS}\"\n"
    )
    
    # Also create in output directory if it exists
    if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        set(OUTPUT_LLDBINIT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/.lldbinit")
        file(WRITE "${OUTPUT_LLDBINIT}"
            "# Auto-generated LLDB init file for Kosma project\n"
            "# This file is automatically loaded when LLDB starts in this directory\n\n"
            "# Load KSString visualizer\n"
            "command script import \"${VISUALIZER_SCRIPT_ABS}\"\n"
        )
    endif()
    
    message(STATUS "Global LLDB visualizers configured")
    message(STATUS "  Visualizer script: ${VISUALIZER_SCRIPT_ABS}")
    message(STATUS "  Global LLDB init file: ${GLOBAL_LLDBINIT}")
    if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        message(STATUS "  Output LLDB init file: ${OUTPUT_LLDBINIT}")
    endif()
    message(STATUS "  LLDB will automatically load this when started in these directories")
endfunction()


#!/usr/bin/env python3
"""
Test script to verify KSString visualizer is working in LLDB.

Usage in LLDB:
    (lldb) command script import Configs/Debugger/test_visualizer.py
    (lldb) test_ksstring_visualizer
"""

import lldb


def test_ksstring_visualizer(debugger, command, result, internal_dict):
    """Test if KSString visualizer is registered."""
    target = debugger.GetSelectedTarget()
    if not target:
        result.AppendMessage("Error: No target selected")
        return
    
    # Check if visualizer is registered
    type_names = [
        "Kosma::Core::String",
        "String",
        "Kosma::Core::String &",
        "const Kosma::Core::String",
        "const Kosma::Core::String &",
    ]
    
    result.AppendMessage("Checking KSString visualizer registration...")
    for type_name in type_names:
        # Try to get summary format
        interp = debugger.GetCommandInterpreter()
        result_obj = lldb.SBCommandReturnObject()
        interp.HandleCommand(f'type summary list "{type_name}"', result_obj)
        if result_obj.Succeeded() and type_name in result_obj.GetOutput():
            result.AppendMessage(f"  ✓ Registered for: {type_name}")
        else:
            result.AppendMessage(f"  ✗ Not registered for: {type_name}")


def __lldb_init_module(debugger, internal_dict):
    """Register the test command."""
    debugger.HandleCommand(
        'command script add -f test_visualizer.test_ksstring_visualizer test_ksstring_visualizer'
    )
    print("[KSString Visualizer Test] Command 'test_ksstring_visualizer' registered")










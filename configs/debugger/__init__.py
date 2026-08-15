#!/usr/bin/env python3
"""
Unified loader for Kosma LLDB visualizers.
This module automatically loads all visualizers when imported.
"""

import lldb
import os

# Get the directory where this script is located
_DEBUGGER_DIR = os.path.dirname(os.path.abspath(__file__))

def __lldb_init_module(debugger, internal_dict):
    """
    Initialize all visualizers when this module is loaded.
    This function automatically loads KSString and KSVector visualizers.
    """
    try:
        # Load KSString visualizer
        ksstring_path = os.path.join(_DEBUGGER_DIR, "KSString.py")
        if os.path.exists(ksstring_path):
            debugger.HandleCommand(f'command script import "{ksstring_path}"')
        
        # Load KSVector visualizer
        ksvector_path = os.path.join(_DEBUGGER_DIR, "KSVector.py")
        if os.path.exists(ksvector_path):
            debugger.HandleCommand(f'command script import "{ksvector_path}"')
        
        print("[Kosma Visualizers] All visualizers loaded successfully")
    except Exception as e:
        print(f"[Kosma Visualizers] Error loading visualizers: {e}")


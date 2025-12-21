#!/usr/bin/env python3
"""
LLDB Visualizer for Kosma::Core::String (KSString)

This script provides a visualizer for the KSString class in LLDB debugger.
It handles both SSO (Small String Optimization) and heap-allocated strings.
"""

import lldb
import sys


def _get_union_is_long(valobj):
    """
    Get the isLong flag from the union.
    CRITICAL: In a union, m_heap.isLong and m_sso.isLong are at DIFFERENT memory locations!
    We MUST read m_heap.isLong to determine the mode, because:
    - m_heap.isLong == 1 means heap mode (use m_heap)
    - m_heap.isLong == 0 means SSO mode (use m_sso)
    
    DO NOT read m_sso.isLong when in heap mode - it will give wrong results!
    
    Returns (is_long_value, success)
    """
    # ALWAYS read m_heap.isLong first - this is the authoritative flag
    heap_struct = valobj.GetChildMemberWithName('m_heap')
    if heap_struct.IsValid():
        is_long_field = heap_struct.GetChildMemberWithName('isLong')
        if is_long_field.IsValid():
            is_long = is_long_field.GetValueAsUnsigned()
            return (is_long, True)
    
    # Try direct memory read as fallback
    # m_heap layout: Char* data (8 bytes) + size_t length (8 bytes) + size_t (capacity:63, isLong:1) (8 bytes)
    # So isLong is at offset 16 (data) + 8 (length) = 24 bytes, last bit of that 8-byte value
    try:
        process = valobj.GetProcess()
        if process.IsValid():
            union_addr = valobj.GetLoadAddress()
            if union_addr != lldb.LLDB_INVALID_ADDRESS:
                error = lldb.SBError()
                # Read the 8-byte value at offset 24 (capacity + isLong)
                capacity_addr = union_addr + 24
                capacity_and_flag = process.ReadUnsignedFromMemory(capacity_addr, 8, error)
                if error.Success():
                    # isLong is the last bit
                    is_long = capacity_and_flag & 0x01
                    return (is_long, True)
    except Exception:
        pass
    
    return (None, False)


def __lldb_init_module(debugger, internal_dict):
    """Initialize the visualizer when the module is loaded."""
    # Ensure our category is enabled, otherwise the providers may never run.
    try:
        debugger.HandleCommand('type category enable Kosma')
    except Exception:
        pass

    # Try multiple type name formats
    type_names = [
        "Kosma::Core::String",
        "String",
        "Kosma::Core::String &",
        "const Kosma::Core::String",
        "const Kosma::Core::String &",
    ]
    
    # First, clear any existing providers that might interfere
    for type_name in type_names:
        try:
            # Clear existing summary providers
            debugger.HandleCommand(f'type summary delete "{type_name}"')
        except Exception:
            pass
        try:
            # Clear existing synthetic children providers
            debugger.HandleCommand(f'type synthetic delete "{type_name}"')
        except Exception:
            pass
    
    registered_count = 0
    for type_name in type_names:
        try:
            # Register the summary provider
            # Use -C option to show summary even when expanded
            # Do NOT use -h option as it may prevent summary from showing
            # Use -x option to make it work with expression evaluation
            cmd = f'type summary add -F KSString.ksstring_summary_provider "{type_name}" -w Kosma -C -x'
            debugger.HandleCommand(cmd)
            # Register synthetic children provider to show data array in heap mode
            # This allows expanding m_heap.data to show individual characters
            # Note: This will replace the default children display, but we can provide
            # both the original fields and the data array expansion
            debugger.HandleCommand(
                f'type synthetic add -l KSString.KSStringSyntheticChildrenProvider "{type_name}" -w Kosma'
            )
            
            registered_count += 1
        except Exception:
            # Continue trying other type names
            pass
    
    # Register summary provider for Char type to display m_ch value
    try:
        debugger.HandleCommand('type summary add -F KSString.char_summary_provider "Kosma::Core::Char" -w Kosma')
        debugger.HandleCommand('type summary add -F KSString.char_summary_provider "const Kosma::Core::Char" -w Kosma')
    except Exception as e:
        pass
    
    # Print confirmation message
    if registered_count > 0:
        print(f"[KSString Visualizer] Successfully registered for {registered_count} type name(s)")
        print(f"[KSString Visualizer] Note: Summary provider is registered and should work with:")
        print(f"[KSString Visualizer]   - 'po <var>' (print object) - should show summary")
        print(f"[KSString Visualizer]   - 'frame variable <var>' - may show expanded view")
        print(f"[KSString Visualizer]   - VS Code debugger variables window - should show summary")
    else:
        print("[KSString Visualizer] Warning: Failed to register for any type names")


def ksstring_summary_provider(valobj, internal_dict):
    """
    Summary provider for KSString.
    Returns a string representation of the KSString object.
    """
    try:
        # Debug: Print the type name
        type_name = valobj.GetTypeName()
        print(f"[KSString] Visualizer called for type: {type_name}")
        
        if not valobj.IsValid():
            return '<invalid KSString object>'
        
        # Handle reference and pointer types: need to dereference first
        # For "const Kosma::Core::String &" or "Kosma::Core::String *", we need to get the actual object
        # CRITICAL: Use GetNonSyntheticValue() FIRST to avoid synthetic children provider interference
        actual_valobj = valobj
        if type_name and ('&' in type_name or '*' in type_name):
            # Try multiple methods to get the actual object (prioritize GetNonSyntheticValue)
            candidates = []
            try:
                # Method 1: GetNonSyntheticValue() - CRITICAL: This bypasses synthetic children
                non_synth = valobj.GetNonSyntheticValue()
                if non_synth.IsValid():
                    candidates.append(("GetNonSyntheticValue()", non_synth))
            except Exception:
                pass
            
            try:
                # Method 2: For references, GetChildAtIndex(0) usually works
                if '&' in type_name:
                    child = valobj.GetChildAtIndex(0)
                    if child.IsValid():
                        candidates.append(("GetChildAtIndex(0)", child))
            except Exception:
                pass
            
            try:
                # Method 3: Try Dereference() (but this may return synthetic children)
                deref = valobj.Dereference()
                if deref.IsValid():
                    # Check if this is synthetic (has too many children = likely synthetic)
                    num_children = deref.GetNumChildren()
                    if num_children <= 5:  # Real object should have few children (union + charCount)
                        candidates.append(("Dereference()", deref))
            except Exception:
                pass
            
            # Choose the first valid candidate, or fallback to original
            if candidates:
                method_name, candidate = candidates[0]
                actual_valobj = candidate
            else:
                # If all methods failed, use original valobj
                actual_valobj = valobj
        else:
            # Not a reference or pointer, but still try GetNonSyntheticValue to avoid synthetic
            try:
                non_synth = valobj.GetNonSyntheticValue()
                if non_synth.IsValid():
                    actual_valobj = non_synth
            except Exception:
                pass
        
        # Get union members from the actual object
        # Try multiple ways to access union members
        sso_struct = None
        heap_struct = None
        
        # Method 1: Direct member access
        try:
            sso_struct = actual_valobj.GetChildMemberWithName('m_sso')
            heap_struct = actual_valobj.GetChildMemberWithName('m_heap')
        except Exception:
            pass
        
        # Method 2: If direct access failed, try by index
        if (not sso_struct or not sso_struct.IsValid()) or (not heap_struct or not heap_struct.IsValid()):
            try:
                num_children = actual_valobj.GetNumChildren()
                # List all children to find union
                for i in range(num_children):
                    try:
                        child = actual_valobj.GetChildAtIndex(i)
                        if child.IsValid():
                            child_name = child.GetName()
                            if child_name == 'm_sso' and (not sso_struct or not sso_struct.IsValid()):
                                sso_struct = child
                            if child_name == 'm_heap' and (not heap_struct or not heap_struct.IsValid()):
                                heap_struct = child
                    except Exception:
                        pass
            except Exception:
                pass
        
        # Fallback: if still not found, try original valobj
        if (not sso_struct or not sso_struct.IsValid()) or (not heap_struct or not heap_struct.IsValid()):
            try:
                if not sso_struct or not sso_struct.IsValid():
                    sso_struct = valobj.GetChildMemberWithName('m_sso')
                if not heap_struct or not heap_struct.IsValid():
                    heap_struct = valobj.GetChildMemberWithName('m_heap')
            except Exception:
                pass
        
        if not heap_struct.IsValid():
            print("[KSString] ERROR: Cannot access m_heap struct")
            return '<KSString: cannot access m_heap>'
        
        # CRITICAL: Directly read m_heap.isLong - this is the authoritative flag
        is_long_field = heap_struct.GetChildMemberWithName('isLong')
        if not is_long_field.IsValid():
            print("[KSString] ERROR: Cannot access m_heap.isLong field")
            return '<KSString: cannot access m_heap.isLong>'
        
        is_long = is_long_field.GetValueAsUnsigned()
        print(f"[KSString] Read m_heap.isLong = {is_long}")
        
        # Determine mode
        is_heap_mode = (is_long == 1)
        is_sso_mode = (is_long == 0)
        
        print(f"[KSString] Mode: is_heap={is_heap_mode}, is_sso={is_sso_mode}")
        
        # IMPORTANT: Handle heap mode FIRST, because if isLong=1, we MUST use heap mode
        # Handle Heap mode
        if is_heap_mode:
            # Heap mode
            if heap_struct.IsValid():
                data_ptr = heap_struct.GetChildMemberWithName('data')
                length_field = heap_struct.GetChildMemberWithName('length')
                if data_ptr.IsValid() and length_field.IsValid():
                    length = length_field.GetValueAsUnsigned()
                    data_addr = data_ptr.GetValueAsUnsigned()
                    if length > 0 and data_addr != 0:
                        # Read the string data
                        process = actual_valobj.GetProcess()
                        if process.IsValid():
                            error = lldb.SBError()
                            chars = []
                            
                            # Get Char type size
                            target = actual_valobj.GetTarget()
                            char_type = target.FindFirstType('Kosma::Core::Char')
                            if char_type.IsValid():
                                char_size = char_type.GetByteSize()
                            else:
                                # Fallback: Char contains U16Char (2 bytes) + possible padding
                                char_size = 2  # Minimum size for U16Char
                            
                            for i in range(length):
                                try:
                                    # Read the m_ch member of Char struct
                                    # m_ch is at offset 0 in Char class
                                    char_addr = data_addr + i * char_size
                                    
                                    # Try to read as U16Char (2 bytes)
                                    u16_val = process.ReadUnsignedFromMemory(char_addr, 2, error)
                                    if error.Success():
                                        if u16_val <= 0x7F:
                                            chars.append(chr(u16_val))
                                        elif u16_val <= 0xFFFF:
                                            try:
                                                chars.append(chr(u16_val))
                                            except:
                                                chars.append(f'\\u{u16_val:04X}')
                                        else:
                                            chars.append(f'\\u{u16_val:04X}')
                                    else:
                                        chars.append('?')
                                        break
                                except Exception:
                                    chars.append('?')
                                    break
                            
                            result = ''.join(chars)
                            # Escape special characters for display
                            result = result.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n').replace('\r', '\\r').replace('\t', '\\t')
                            summary = f'"{result}" (heap, length={length})'
                            return summary
                    else:
                        summary = '"" (heap, empty)'
                        return summary
        
        # Handle SSO mode (only if isLong == 0)
        if is_sso_mode:
            # SSO mode
            if sso_struct.IsValid():
                buffer = sso_struct.GetChildMemberWithName('buffer')
                size_field = sso_struct.GetChildMemberWithName('size')
                if size_field.IsValid() and buffer.IsValid():
                    size = size_field.GetValueAsUnsigned()
                    if size > 0:
                        # Extract characters from buffer
                        chars = []
                        for i in range(size):
                            try:
                                char_elem = buffer.GetChildAtIndex(i)
                                if char_elem.IsValid():
                                    # Char is a class with m_ch member (U16Char)
                                    char_val = char_elem.GetChildMemberWithName('m_ch')
                                    if char_val.IsValid():
                                        u16_val = char_val.GetValueAsUnsigned()
                                        # Convert to string
                                        if u16_val <= 0x7F:
                                            chars.append(chr(u16_val))
                                        elif u16_val <= 0xFFFF:
                                            try:
                                                chars.append(chr(u16_val))
                                            except:
                                                chars.append(f'\\u{u16_val:04X}')
                                        else:
                                            chars.append(f'\\u{u16_val:04X}')
                                    else:
                                        # Try direct memory read if m_ch access fails
                                        char_addr = char_elem.GetLoadAddress()
                                        if char_addr != lldb.LLDB_INVALID_ADDRESS:
                                            process = actual_valobj.GetProcess()
                                            if process.IsValid():
                                                error = lldb.SBError()
                                                # m_ch is at offset 0 in Char class
                                                u16_val = process.ReadUnsignedFromMemory(char_addr, 2, error)
                                                if error.Success():
                                                    if u16_val <= 0x7F:
                                                        chars.append(chr(u16_val))
                                                    else:
                                                        chars.append(f'\\u{u16_val:04X}')
                                                else:
                                                    chars.append('?')
                                        else:
                                            chars.append('?')
                                else:
                                    chars.append('?')
                            except Exception:
                                chars.append('?')
                        
                        result = ''.join(chars)
                        # Escape special characters for display
                        result = result.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n').replace('\r', '\\r').replace('\t', '\\t')
                        summary = f'"{result}" (SSO, length={size})'
                        return summary
                    else:
                        summary = '"" (SSO, empty)'
                        return summary
        
        summary = f'<KSString: unknown mode, type={type_name}, isLong={is_long}>'
        return summary
    except Exception as e:
        error_msg = str(e)
        summary = f'<error: {error_msg}>'
        return summary


def char_summary_provider(valobj, internal_dict):
    """
    Summary provider for Char type.
    Returns the Unicode code point value.
    """
    try:
        if not valobj.IsValid():
            return '<invalid Char>'
        
        # Get the m_ch member
        m_ch = valobj.GetChildMemberWithName('m_ch')
        if m_ch.IsValid():
            ch_value = m_ch.GetValueAsUnsigned()
            # Format as Unicode code point
            if ch_value <= 0x7F:
                # ASCII
                return f"U+{ch_value:04X} u'{chr(ch_value)}'"
            else:
                # Unicode
                try:
                    # Try to decode as UTF-16 surrogate pair or single character
                    if 0xD800 <= ch_value <= 0xDFFF:
                        return f"U+{ch_value:04X} (surrogate)"
                    else:
                        return f"U+{ch_value:04X}"
                except Exception:
                    return f"U+{ch_value:04X}"
        return '<no m_ch>'
    except Exception:
        return '<error>'


class KSStringSyntheticChildrenProvider:
    """
    Synthetic children provider for KSString.
    Provides access to individual characters and metadata.
    """
    
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj
        self.update()
    
    def update(self):
        """Update the internal state."""
        try:
            # CRITICAL: Always check m_heap.isLong FIRST - this is the authoritative flag
            # In a union, m_heap.isLong and m_sso.isLong are at DIFFERENT memory locations
            # We MUST read m_heap.isLong to determine the mode
            heap_struct = self.valobj.GetChildMemberWithName('m_heap')
            if heap_struct.IsValid():
                is_long_field = heap_struct.GetChildMemberWithName('isLong')
                if is_long_field.IsValid():
                    is_long = is_long_field.GetValueAsUnsigned()
                    
                    # Heap mode (isLong == 1)
                    if is_long == 1:
                        length_field = heap_struct.GetChildMemberWithName('length')
                        if length_field.IsValid():
                            self.mode = 'heap'
                            self.length = length_field.GetValueAsUnsigned()
                            self.data_ptr = heap_struct.GetChildMemberWithName('data')
                            self.children = []
                            return
                    
                    # SSO mode (isLong == 0)
                    elif is_long == 0:
                        sso_struct = self.valobj.GetChildMemberWithName('m_sso')
                        if sso_struct.IsValid():
                            buffer = sso_struct.GetChildMemberWithName('buffer')
                            size_field = sso_struct.GetChildMemberWithName('size')
                            if size_field.IsValid():
                                size = size_field.GetValueAsUnsigned()
                                self.mode = 'SSO'
                                self.length = size
                                self.buffer = buffer
                                self.children = []
                                return
            
            self.children = []
            self.length = 0
        except Exception:
            self.children = []
            self.length = 0
    
    def num_children(self):
        """Return the number of children."""
        mode = getattr(self, 'mode', None)
        length = getattr(self, 'length', 0)
        if mode == 'heap' or mode == 'SSO':
            return length
        return 0
    
    def has_children(self):
        """Check if there are children."""
        return self.num_children() > 0
    
    def get_child_index(self, name):
        """Get the index of a child by name."""
        try:
            if name.startswith('[') and name.endswith(']'):
                index = int(name[1:-1])
                if 0 <= index < self.num_children():
                    return index
        except ValueError:
            pass
        return None
    
    def get_child_at_index(self, index):
        """Get a child at the given index."""
        if index >= self.num_children():
            return None
        
        try:
            if self.mode == 'SSO' and hasattr(self, 'buffer'):
                char_elem = self.buffer.GetChildAtIndex(index)
                if char_elem.IsValid():
                    return char_elem.CreateChildAtOffset(f'[{index}]', 0, char_elem.GetType())
            elif self.mode == 'heap' and hasattr(self, 'data_ptr') and self.data_ptr.IsValid():
                # Heap mode: read character from data pointer
                # Instead of creating individual children, we create a child that represents the data array
                # This allows LLDB to expand m_heap.data as an array
                try:
                    # Get the data pointer value object
                    data_ptr_valobj = self.data_ptr
                    if data_ptr_valobj.IsValid():
                        # Get Char type
                        target = self.valobj.GetTarget()
                        char_type = target.FindFirstType('Kosma::Core::Char')
                        if char_type.IsValid():
                            char_size = char_type.GetByteSize()
                        else:
                            char_size = 2  # Fallback
                        
                        # Calculate offset for this character
                        offset = index * char_size
                        
                        # Create a child at the offset from the data pointer
                        # This creates a value that points to data[index]
                        data_addr = data_ptr_valobj.GetValueAsUnsigned()
                        if data_addr != 0:
                            char_addr = data_addr + offset
                            
                            # Use CreateValueFromAddress - but we need to create SBAddress properly
                            # ResolveLoadAddress returns an SBAddress directly, not a boolean
                            process = self.valobj.GetProcess()
                            if process.IsValid():
                                # Try to resolve the address using SBTarget
                                # ResolveLoadAddress(load_addr) returns SBAddress
                                addr = target.ResolveLoadAddress(char_addr)
                                if addr.IsValid():
                                    char_valobj = target.CreateValueFromAddress(
                                        f'[{index}]', addr, char_type
                                    )
                                    if char_valobj.IsValid():
                                        # Try to get the m_ch member directly for display
                                        # This might help LLDB display the value correctly
                                        m_ch = char_valobj.GetChildMemberWithName('m_ch')
                                        if m_ch.IsValid():
                                            # Try to create a char16_t value directly from the address
                                            # This should be more reliable for LLDB to display
                                            try:
                                                # Get the U16Char type (char16_t)
                                                u16char_type = target.FindFirstType('char16_t')
                                                if not u16char_type.IsValid():
                                                    # Try alternative names
                                                    u16char_type = target.FindFirstType('unsigned short')
                                                if u16char_type.IsValid() and addr.IsValid():
                                                    # Create a value from the address using char16_t type
                                                    u16_value = target.CreateValueFromAddress(
                                                        f'[{index}]', addr, u16char_type
                                                    )
                                                    if u16_value.IsValid():
                                                        return u16_value
                                            except Exception:
                                                pass
                                            # Fallback: return m_ch directly
                                            return m_ch
                                        # Fallback: return the Char object itself
                                        return char_valobj
                                else:
                                    # Fallback: try creating address directly
                                    try:
                                        # Create SBAddress from load address directly
                                        addr = lldb.SBAddress(char_addr, target)
                                        if addr.IsValid():
                                            char_valobj = target.CreateValueFromAddress(
                                                f'[{index}]', addr, char_type
                                            )
                                            if char_valobj.IsValid():
                                                return char_valobj
                                    except Exception:
                                        pass
                except Exception:
                    pass
        except Exception:
            pass
        
        return None


class CharPointerSyntheticChildrenProvider:
    """
    Synthetic children provider for Char* pointers.
    This allows Char* pointers to expand as arrays when they are part of a String object.
    Note: This is a simplified version that may not work in all contexts.
    """
    
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj
        self.length = 0
        self.update()
    
    def update(self):
        """Update the internal state."""
        # For Char* pointers, we can't determine the length from the pointer itself
        # This provider is mainly a placeholder - actual expansion should be handled
        # by the String's synthetic children provider
        self.length = 0
    
    def num_children(self):
        """Return the number of children."""
        return self.length
    
    def has_children(self):
        """Check if there are children."""
        return self.length > 0
    
    def get_child_index(self, name):
        """Get the index of a child by name."""
        return None
    
    def get_child_at_index(self, index):
        """Get a child at the given index."""
        return None


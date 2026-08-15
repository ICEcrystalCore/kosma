#!/usr/bin/env python3
"""
LLDB Visualizer for Kosma::Core::Vector (KSVector)

This script provides a visualizer for the KSVector template class in LLDB debugger.
It displays the vector size, capacity, and allows expanding to view individual elements.
"""

import lldb
import sys


def __lldb_init_module(debugger, internal_dict):
    """Initialize the visualizer when the module is loaded."""
    # Ensure our category is enabled
    try:
        debugger.HandleCommand('type category enable Kosma')
    except Exception:
        pass

    # For template types, we need to register with a pattern
    # LLDB supports template type matching with wildcards
    type_patterns = [
        "Kosma::Core::Vector<*",
        "Vector<*",
    ]
    
    # First, clear any existing providers
    for pattern in type_patterns:
        try:
            debugger.HandleCommand(f'type summary delete "{pattern}"')
        except Exception:
            pass
        try:
            debugger.HandleCommand(f'type synthetic delete "{pattern}"')
        except Exception:
            pass
    
    registered_count = 0
    for pattern in type_patterns:
        try:
            # Register the summary provider
            # Use -C to show summary even when expanded
            # Use -x to work with expression evaluation
            cmd = f'type summary add -F KSVector.ksvector_summary_provider "{pattern}" -w Kosma -x'
            debugger.HandleCommand(cmd)
            
            # Register synthetic children provider to show elements
            debugger.HandleCommand(
                f'type synthetic add -l KSVector.KSVectorSyntheticChildrenProvider "{pattern}" -w Kosma'
            )
            
            registered_count += 1
        except Exception:
            pass
    
    # Print confirmation message
    if registered_count > 0:
        print(f"[KSVector Visualizer] Successfully registered for {registered_count} type pattern(s)")
    else:
        print("[KSVector Visualizer] Warning: Failed to register for any type patterns")


def ksvector_summary_provider(valobj, internal_dict):
    """
    Summary provider for KSVector.
    Returns a string representation of the Vector object.
    """
    try:
        if not valobj.IsValid():
            return '<invalid Vector>'
        
        # Handle reference and pointer types
        type_name = valobj.GetTypeName()
        actual_valobj = valobj
        
        if type_name and ('&' in type_name or '*' in type_name):
            # Try to get the actual object
            try:
                non_synth = valobj.GetNonSyntheticValue()
                if non_synth.IsValid():
                    actual_valobj = non_synth
            except Exception:
                try:
                    if '&' in type_name:
                        child = valobj.GetChildAtIndex(0)
                        if child.IsValid():
                            actual_valobj = child
                except Exception:
                    pass
        
        # Get vector members
        m_data = actual_valobj.GetChildMemberWithName('m_data')
        m_size = actual_valobj.GetChildMemberWithName('m_size')
        m_capacity = actual_valobj.GetChildMemberWithName('m_capacity')
        
        if not m_size.IsValid() or not m_capacity.IsValid():
            return '<Vector: cannot access members>'
        
        size = m_size.GetValueAsUnsigned()
        capacity = m_capacity.GetValueAsUnsigned()
        
        # Get element type from template
        element_type_str = "T"
        if '<' in type_name and '>' in type_name:
            # Extract template parameter
            start = type_name.find('<')
            end = type_name.rfind('>')
            if start != -1 and end != -1:
                element_type_str = type_name[start+1:end].strip()
        
        # Build summary
        if size == 0:
            summary = f'Vector<{element_type_str}>(size=0, capacity={capacity})'
        else:
            # Show size and capacity
            summary = f'Vector<{element_type_str}>(size={size}, capacity={capacity})'
        
        return summary
    except Exception as e:
        return f'<error: {str(e)}>'


class KSVectorSyntheticChildrenProvider:
    """
    Synthetic children provider for KSVector.
    Provides access to individual elements.
    """
    
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj
        self.update()
    
    def update(self):
        """Update the internal state."""
        try:
            # Handle reference and pointer types
            type_name = self.valobj.GetTypeName()
            actual_valobj = self.valobj
            
            if type_name and ('&' in type_name or '*' in type_name):
                try:
                    non_synth = self.valobj.GetNonSyntheticValue()
                    if non_synth.IsValid():
                        actual_valobj = non_synth
                except Exception:
                    try:
                        if '&' in type_name:
                            child = self.valobj.GetChildAtIndex(0)
                            if child.IsValid():
                                actual_valobj = child
                    except Exception:
                        pass
            
            # Get vector members
            m_data = actual_valobj.GetChildMemberWithName('m_data')
            m_size = actual_valobj.GetChildMemberWithName('m_size')
            m_capacity = actual_valobj.GetChildMemberWithName('m_capacity')
            
            if m_size.IsValid() and m_capacity.IsValid():
                self.size = m_size.GetValueAsUnsigned()
                self.capacity = m_capacity.GetValueAsUnsigned()
                self.data_ptr = m_data
                self.actual_valobj = actual_valobj
                
                # Extract element type from template
                self.element_type_str = "T"
                if '<' in type_name and '>' in type_name:
                    start = type_name.find('<')
                    end = type_name.rfind('>')
                    if start != -1 and end != -1:
                        self.element_type_str = type_name[start+1:end].strip()
            else:
                self.size = 0
                self.capacity = 0
                self.data_ptr = None
        except Exception:
            self.size = 0
            self.capacity = 0
            self.data_ptr = None
    
    def num_children(self):
        """Return the number of children (elements)."""
        return getattr(self, 'size', 0)
    
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
            if not hasattr(self, 'data_ptr') or not self.data_ptr or not self.data_ptr.IsValid():
                return None
            
            data_addr = self.data_ptr.GetValueAsUnsigned()
            if data_addr == 0:
                return None
            
            # Get element type from m_data pointer's pointee type
            target = self.valobj.GetTarget()
            data_type = self.data_ptr.GetType()
            
            if not data_type.IsValid():
                return None
            
            # Get the pointee type (this is the element type T)
            element_type = data_type.GetPointeeType()
            
            if not element_type.IsValid():
                return None
            
            # Calculate address of element at index
            element_size = element_type.GetByteSize()
            element_addr = data_addr + index * element_size
            
            # Resolve address and create value
            addr = target.ResolveLoadAddress(element_addr)
            if addr.IsValid():
                element_valobj = target.CreateValueFromAddress(
                    f'[{index}]', addr, element_type
                )
                if element_valobj.IsValid():
                    return element_valobj
            else:
                # Fallback: try creating address directly
                try:
                    addr = lldb.SBAddress(element_addr, target)
                    if addr.IsValid():
                        element_valobj = target.CreateValueFromAddress(
                            f'[{index}]', addr, element_type
                        )
                        if element_valobj.IsValid():
                            return element_valobj
                except Exception:
                    pass
        except Exception:
            pass
        
        return None


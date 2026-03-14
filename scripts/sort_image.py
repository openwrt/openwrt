#!/usr/bin/env python3
import os
import sys
import re
import subprocess

def get_file_info(filepath):
    """Executes the 'file' utility to get raw metadata for the given file."""
    try:
        # -r: raw, -N: no padding, -F '': no separator for cleaner parsing
        result = subprocess.check_output(
            ['file', '-r', '-N', '-F', '', filepath], 
            stderr=subprocess.STDOUT, 
            universal_newlines=True
        )
        return result
    except subprocess.Called_ProcessError:
        return ""

def is_executable_elf(filepath):
    """Checks for +x permission and ELF format."""
    if os.access(filepath, os.X_OK):
        info = get_file_info(filepath)
        return "ELF" in info
    return False

def is_shell_script(filepath):
    """Checks if the file is a POSIX shell script via 'file' utility."""
    info = get_file_info(filepath)
    return "shell script" in info.lower() or "POSIX shell script" in info

# Function mapping for the '!' instruction
FUNCTIONS = {
    "executable": is_executable_elf,
    "shell_script": is_shell_script
}

def main():
    if len(sys.argv) < 4:
        print("Usage: ./sort_image.py <scan_dir> <template_file> <unsorted_file>")
        sys.exit(1)

    scan_dir = sys.argv[1]
    template_path = sys.argv[2]
    unsorted_output = sys.argv[3]

    # 1. Recursive scan of the directory - FILTERING START
    all_files = []
    for root, _, files in os.walk(scan_dir):
        for f in files:
            full_path = os.path.join(root, f)
            
            # CRITICAL FILTERS: 
            # 1. Skip symbolic links (islink check must come first)
            # 2. Ensure it's a regular file (not a pipe, socket, or device)
            if os.path.islink(full_path):
                continue
            if not os.path.isfile(full_path):
                continue

            rel_path = os.path.relpath(full_path, scan_dir)
            all_files.append({'abs': full_path, 'rel': rel_path})
    # FILTERING END

    sorted_list = []

    # 2. Process the template line by line
    try:
        with open(template_path, 'r') as t:
            for line in t:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue

                parts = line.split()
                if len(parts) < 3:
                    continue

                cmd = parts[0]      # ! or @
                pattern = parts[1]  # func name or regex
                priority = parts[2] # priority number

                matched_files = []

                for f_obj in all_files:
                    is_match = False
                    
                    if cmd == '!':
                        func = FUNCTIONS.get(pattern)
                        if func and func(f_obj['abs']):
                            is_match = True
                    
                    elif cmd == '@':
                        try:
                            if re.search(pattern, f_obj['rel']):
                                is_match = True
                        except re.error:
                            continue

                    if is_match:
                        sorted_list.append(f"{f_obj['rel']} {priority}")
                        matched_files.append(f_obj)

                for match in matched_files:
                    all_files.remove(match)

    except FileNotFoundError:
        print(f"Error: Template file '{template_path}' not found.")
        sys.exit(1)

    for entry in sorted_list:
        print(entry)

    with open(unsorted_output, 'w') as u:
        for f_obj in all_files:
            u.write(f_obj['rel'] + '\n')

if __name__ == "__main__":
    main()

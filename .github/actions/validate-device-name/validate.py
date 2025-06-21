#!/usr/bin/env python3
import os
import subprocess
import sys
from typing import List, Tuple

def run_make():
    cmd = [
        'make',
        '-C', f'target/linux/{os.environ["TARGET"]}',
        '--no-print-directory',
        'DUMP=1',
        'TARGET_BUILD=1',
        f'SUBTARGET={os.environ["SUBTARGET"]}',
        'V=s'
    ]
    env = os.environ.copy()
    env['TOPDIR'] = os.getcwd()

    with subprocess.Popen(cmd, env=env, stdout=subprocess.PIPE, text=True) as proc:
        for line in proc.stdout:
            yield line.strip()

def process_devices():
    profile = ""
    name = ""
    alt_names = []
    in_alts = False

    for line in run_make():
        if line == "@@":
            if profile and name and not any(name.startswith(alt) for alt in alt_names):
                yield profile
                yield name
                yield from alt_names

            profile = ""
            name = ""
            alt_names = []
            in_alts = False
        elif line.startswith("Target-Profile: DEVICE_"):
            profile = line.split("DEVICE_", 1)[1]
        elif line.startswith("Target-Profile-Name: "):
            name = line.split(": ", 1)[1]
        elif line == "Alternative device titles:":
            in_alts = True
        elif in_alts and line.startswith("- "):
            alt_names.append(line[2:])

if __name__ == "__main__":
    device = os.environ["DEVICE"]
    devices = list(process_devices())

    if device in devices:
        print("::notice::Device name found", file=sys.stderr)
        print("valid=true")
        sys.exit(0)

    import difflib
    matches = difflib.get_close_matches(device, devices, n=5, cutoff=0.5)

    if matches:
        print("error<<EOF")
        print(f"Device not found. Did you mean one of these?")
        for match in matches:
            print(f"  - {match}")
        print("EOF")
    else:
        print("error=Device not found.")

    print(f"::notice::Device '{device}' not found", file=sys.stderr)
    print("valid=false")

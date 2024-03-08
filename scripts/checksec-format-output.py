#!/usr/bin/env python3

from pathlib import Path
from sys import argv
import json

if len(argv) != 6:
    print("Usage: checkseck_format_output.py input build_dir package_name package_version output")
    exit(1)

input_path = Path(argv[1])
builddir_path = argv[2]
name = argv[3]
version = argv[4]
output_path = Path(argv[5])
output={}

with open(input_path, "r") as read_file:
    output["elflist"]={}
    data = json.load(read_file)
    del data['dir']
    for k in list(data):
        new=k.replace(builddir_path, "")
        output["elflist"][new]=data.pop(k)
        output["elflist"][new]["rpath"]="no"
        output["elflist"][new]["filename"] = output["elflist"][new]["filename"].replace(builddir_path, "")


    
    output['package'] = name
    output['version'] = version
    with open(output_path, "w") as write_file:
        json.dump(output, write_file)


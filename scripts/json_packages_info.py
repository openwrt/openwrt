#!/usr/bin/env python3
import os
import sys
import json
import argparse


def _has_key_in_line(line):
    return ": " in line


def _process_key_in_line(line, package):
    key, value = line.split(": ", 1)
    value = modify_value(key.strip(), value.strip())
    package[key.strip()] = value


def modify_value(key, value):
    """Modify the value for specific keys if appropriate"""
    if key == "Depends":
        return value.split(", ")
    if key == "License":
        return value.split()
    if key == "LicenseFiles":
        return value.split()
    if key == "Conflicts":
        return value.split(", ")
    if key == "Alternatives":
        return value.split(", ")
    if key == "Provides":
        return value.split(", ")

    return value


def generate_manifest(file):
    """Generate a manifest dictionary from the given .manifest file."""
    package_dict = {}

    package = {}
    package_name = None
    key = None

    for line in file:
        line = line.strip()
        if line.startswith("Package: "):
            if package_name and package:
                package_dict[package_name] = package

            package = {}
            _process_key_in_line(line, package)
            package_name = package['Package']

            if package_name in package_dict:
                print("ERROR: It seems there are multiple packages named" +
                      f" '{package_name}'. A package name has to be unique")
                sys.exit(1)
        elif line:
            if _has_key_in_line(line):
                _process_key_in_line(line, package)
            elif package and key:
                package[key] += " " + line

    if package_name and package:
        package_dict[package_name] = package

    return package_dict


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Generate json manifest")
    parser.add_argument("--input-file",
                        dest="input_file",
                        help="Input .manifest file to parse to json.",
                        required=True)
    parser.add_argument("--output-file",
                        dest="output_file",
                        help="Output file for the generated JSON manifest.",
                        required=True)
    args = parser.parse_args()

    if not os.path.exists(args.input_file):
        print(f"ERROR: No manifest file found at {args.input_file}")
        sys.exit(1)

    with open(args.input_file, "r") as file:
        packages_dict = generate_manifest(file)

    with open(args.output_file, "w") as output_file:
        json.dump(packages_dict, output_file)

#!/usr/bin/env python3
"""
Parse the native package index files into a json file for use by downstream
tools.  See:

    https://github.com/openwrt/openwrt/commit/218ce40cd738f3373438aab82467807a8707fb9c

The "version 1" index.json contained ABI-versioned package names, making the
unusable by the ASU server.  The version 2 format contains package names that
have been stripped of their ABI version.
"""

import json


def removesuffix(src, suffix):
    # For compatibility with Python < 3.9.
    suffix_length = len(suffix)
    return src[:-suffix_length] if suffix_length and src.endswith(suffix) else src


def parse_args():
    from argparse import ArgumentParser

    source_format = "apk", "opkg"

    parser = ArgumentParser()
    # fmt: off
    parser.add_argument("-a", "--architecture", required=True,
                        help="Required device architecture: like 'x86_64' or 'aarch64_generic'")
    parser.add_argument("-f", "--source-format", required=True, choices=source_format,
                        help="Required source format of input: 'apk' or 'opkg'")
    parser.add_argument("-m", "--manifest", action="store_true", default=False,
                        help="Print output in opkg list format, as 'package - version' pairs")
    parser.add_argument(dest="source",
                        help="File name for input, '-' for stdin")
    # fmt: on
    args = parser.parse_args()
    return args


def parse_apk(text: str) -> dict:
    packages: dict = {}

    data = json.loads(text)
    if isinstance(data, dict) and "packages" in data:
        # Extract 'apk adbdump' dict field to 'apk query' package list
        data = data["packages"]

    for package in data:
        package_name: str = package["name"]

        if "tags" in package:
            for tag in package["tags"]:
                if tag.startswith("openwrt:abiversion="):
                    # string slicing is faster than split
                    package_abi: str = tag[19:]
                    package_name = removesuffix(package_name, package_abi)
                    break

        packages[package_name] = package["version"]

    return packages


def parse_opkg(text: str) -> dict:
    packages: dict = {}

    # Optimization: using manual string splitting instead of email.parser.Parser()
    # is significantly faster (~14x) for large machine-generated opkg index files
    # as it avoids the overhead of full RFC 822/2822 compliance checks.
    start = 0
    text_len = len(text)

    while start < text_len:
        end = text.find("\n\n", start)
        if end == -1:
            end = text_len

        # Optimization: use string find instead of splitting all lines
        # which provides another ~3x speedup on large index files.
        # It also avoids allocating an intermediate string list.
        p_idx = text.find("Package: ", start, end)
        v_idx = text.find("\nVersion: ", start, end)
        a_idx = text.find("\nABIVersion: ", start, end)

        if p_idx == start:
            p_end = text.find("\n", p_idx, end)
            package_name = text[p_idx+9:p_end if p_end != -1 else end].strip()

            package_version = ""
            if v_idx != -1:
                v_end = text.find("\n", v_idx + 1, end)
                package_version = text[v_idx+10:v_end if v_end != -1 else end].strip()

            if package_name:
                if a_idx != -1:
                    a_end = text.find("\n", a_idx + 1, end)
                    package_abi = text[a_idx+13:a_end if a_end != -1 else end].strip()
                    package_name = removesuffix(package_name, package_abi)
                packages[package_name] = package_version

        start = end + 2

        # Skip extra newlines
        while start < text_len and text[start] == '\n':
            start += 1

    return packages


if __name__ == "__main__":
    import sys

    args = parse_args()

    input = sys.stdin if args.source == "-" else open(args.source, "r")
    with input:
        text: str = input.read()

    packages = parse_apk(text) if args.source_format == "apk" else parse_opkg(text)
    if args.manifest:
        # Emulate the output of 'opkg list' command for compatibility with
        # legacy tooling.
        for name, version in sorted(packages.items()):
            print(name, "-", version)
    else:
        index = {
            "version": 2,
            "architecture": args.architecture,
            "packages": packages,
        }
        print(json.dumps(index, indent=2))

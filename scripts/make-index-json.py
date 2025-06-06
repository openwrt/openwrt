#!/usr/bin/env python3
"""
Parse the native package index files into a json file for use by downstream
tools.  See:

    https://github.com/openwrt/openwrt/commit/218ce40cd738f3373438aab82467807a8707fb9c

The "version 1" index.json contained ABI-versioned package names, making the
unusable by the ASU server.  The version 2 format contains package names that
have been stripped of their ABI version.
"""

import email.parser
import json


def parse_args():
    from argparse import ArgumentParser

    source_format = "apk", "opkg"

    parser = ArgumentParser()
    # fmt: off
    parser.add_argument("-a", "--architecture", required=True,
                        help="Required device architecture: like 'x86_64' or 'aarch64_generic'")
    parser.add_argument("-f", "--source-format", required=True, choices=source_format,
                        help="Required source format of input: 'apk' or 'opkg'")
    parser.add_argument(dest="source",
                        help="File name for input, '-' for stdin")
    # fmt: on
    args = parser.parse_args()
    return args


def parse_apk(text: str) -> dict:
    packages: dict = {}

    data = json.loads(text)
    for package in data.get("packages", []):
        package_name: str = package["name"]

        for tag in package.get("tags", []):
            if tag.startswith("openwrt:abiversion="):
                package_abi: str = tag.split("=")[-1]
                package_name = package_name.removesuffix(package_abi)
                break

        packages[package_name] = package["version"]

    return packages


def parse_opkg(text: str) -> dict:
    packages: dict = {}

    parser: email.parser.Parser = email.parser.Parser()
    chunks: list[str] = text.strip().split("\n\n")
    for chunk in chunks:
        package: dict = parser.parsestr(chunk, headersonly=True)
        package_name: str = package["Package"]
        if package_abi := package.get("ABIVersion"):
            package_name = package_name.removesuffix(package_abi)

        packages[package_name] = package["Version"]

    return packages


if __name__ == "__main__":
    import sys

    args = parse_args()

    input = sys.stdin if args.source == "-" else open(args.source, "r")
    with input:
        text: str = input.read()

    packages = parse_apk(text) if args.source_format == "apk" else parse_opkg(text)
    index = {
        "version": 2,
        "architecture": args.architecture,
        "packages": packages,
    }
    print(json.dumps(index, indent=2))

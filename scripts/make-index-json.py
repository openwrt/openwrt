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
    parser.add_argument("-p", "--packages-manifest", action="store_true", default=False,
                        help="Print output in legacy Packages.manifest (control stanza) format; apk only")
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

        for tag in package.get("tags", []):
            if tag.startswith("openwrt:abiversion="):
                package_abi: str = tag.split("=")[-1]
                package_name = removesuffix(package_name, package_abi)
                break

        packages[package_name] = package["version"]

    return packages


def apk_to_packages_manifest(text: str) -> str:
    """Render 'apk adbdump' JSON as legacy Packages.manifest stanzas.

    Keeps the field names of the opkg-era Packages.manifest so existing
    consumers (e.g. Repology's OpenWrtPackagesParser) work unchanged.
    """

    def one_line(value) -> str:
        if isinstance(value, list):
            value = ", ".join(str(v) for v in value)
        return " ".join(str(value).split())

    data = json.loads(text)
    if isinstance(data, dict) and "packages" in data:
        data = data["packages"]

    stanzas: list = []
    for package in sorted(data, key=lambda p: p["name"]):
        tags: dict = {}
        for tag in package.get("tags", []):
            key, _, value = tag.partition("=")
            tags[key] = value

        arch = package.get("arch", "")
        fields = [
            ("Package", package["name"]),
            ("Version", package["version"]),
            ("Depends", package.get("depends")),
            ("Provides", package.get("provides")),
            ("Source", package.get("origin")),
            ("License", package.get("license")),
            ("Section", tags.get("openwrt:section")),
            ("URL", package.get("url")),
            ("ABIVersion", tags.get("openwrt:abiversion")),
            ("CPE-ID", tags.get("openwrt:cpe")),
            ("Maintainer", package.get("maintainer")),
            ("Architecture", "all" if arch == "noarch" else arch),
            ("Installed-Size", package.get("installed-size")),
            ("Description", package.get("description")),
        ]
        lines = [f"{k}: {one_line(v)}" for k, v in fields if v not in (None, "", [])]
        # Every stanza, including the last, must end with a blank line:
        # Debian-style readers (Repology's included) only emit a package
        # when they hit the blank separator line.
        stanzas.append("\n".join(lines) + "\n\n")

    return "".join(stanzas)


def parse_opkg(text: str) -> dict:
    packages: dict = {}

    parser: email.parser.Parser = email.parser.Parser()
    chunks: list[str] = text.strip().split("\n\n")
    for chunk in chunks:
        package: dict = parser.parsestr(chunk, headersonly=True)
        package_name: str = package["Package"]
        package_abi = package.get("ABIVersion")
        if package_abi:
            package_name = removesuffix(package_name, package_abi)

        packages[package_name] = package["Version"]

    return packages


if __name__ == "__main__":
    import sys

    args = parse_args()

    input = sys.stdin if args.source == "-" else open(args.source, "r")
    with input:
        text: str = input.read()

    if args.packages_manifest:
        if args.source_format != "apk":
            sys.exit("--packages-manifest requires --source-format apk")
        sys.stdout.write(apk_to_packages_manifest(text))
        sys.exit(0)

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

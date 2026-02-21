#!/usr/bin/env python3
"""
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Parse the native package index files into a json file for use by
# downstream tools.
#
"""

import datetime
import email.parser
import json
import uuid


def parse_args():
    from argparse import ArgumentParser

    parser = ArgumentParser()
    # fmt: off

    parser.add_argument(dest="source",
                        help="File name for input, '-' for stdin")
    parser.add_argument("-f", "--source-format", required=True,
                        choices=['apk', 'opkg'],
                        help=("Required source format of"
                              " input: 'apk' or 'opkg'"))
    parser.add_argument("-m", "--manifest",
                        help=("File includes the packages to"
                              " be included in the output"))

    # fmt: on
    args = parser.parse_args()
    return args


def get_apk_sbom(text: str, installed: set) -> list:
    packages: dict = json.loads(text)
    components: list = []

    type_allowed: dict = {
        "kernel": "operating-system",
        "firmware": "firmware",
        "libs": "library"
    }

    for package in packages["packages"]:
        element: dict = {}

        # required
        if 'name' in package:
            name: str = package['name']
            element.update({"name": name})
            if installed:
                if name not in installed:
                    continue

        if 'version' in package:
            element.update({"version": package["version"]})

        for tag in package.get("tags", []):
            if tag.startswith("openwrt:cpe="):
                cpe: str = tag.split("=")[-1]
                element.update({"cpe": cpe})

        # required
        type_category: str = ''

        for tag in package.get("tags", []):
            if tag.startswith("openwrt:section="):
                category: str = tag.split("=")[-1]
                if type_allowed.get(category):
                    type_category = type_allowed.get(category)
        if type_category:
            element.update({"type": type_category})
        else:
            element.update({"type": "application"})

        if 'license' in package:
            licenses: list = []
            for license in package["license"].split():
                licenses.append({"license": {"name": license}})
            element.update({"licenses": licenses})

        components.append(element)

    return components


def get_opkg_sbom(text: str, installed: set) -> list:
    components: list = []

    type_allowed: dict = {
        "kernel": "operating-system",
        "firmware": "firmware",
        "libs": "library"
    }

    parser: email.parser.Parser = email.parser.Parser()
    chunks: list[str] = text.strip().split("\n\n")
    for chunk in chunks:
        element: dict = {}
        package: dict = parser.parsestr(chunk, headersonly=True)

        # required
        if 'Package' in package:
            name: str = package['Package']
            element.update({"name": name})
            if installed:
                if name not in installed:
                    continue

        if 'Version' in package:
            element.update({"version": package['Version']})

        if 'CPE-ID' in package:
            element.update({"cpe": package['CPE-ID']})

        # required
        if 'Section' in package:
            type_category: str = ''
            if type_allowed.get(package['Section']):
                type_category = type_allowed.get(package['Section'])
            if type_category:
                element.update({"type": type_category})
            else:
                element.update({"type": "application"})

        if 'license' in package:
            licenses: list = []
            for license in package["license"].split():
                licenses.append({"license": {"name": license.strip()}})
            element.update({"licenses": licenses})

        if element:
            components.append(element)

    return components


if __name__ == "__main__":
    import sys

    args = parse_args()

    input = sys.stdin if args.source == "-" else open(args.source, "r")
    with input:
        text: str = input.read()

    # Read manifest file (installed packages)
    packages: set = set()
    if args.manifest:
        with open(args.manifest, 'r') as file:
            for line in file:
                packages.add(line.split(' - ')[0].strip())

    components: list = []
    if args.source_format == "apk":
        components = get_apk_sbom(text, packages)
    elif args.source_format == "opkg":
        components = get_opkg_sbom(text, packages)
    else:
        print("Source format unknown")
        raise SystemExit

    timestamp: str = datetime.datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%SZ")
    cyclonedx: dict = {
        "bomFormat": "CycloneDX",
        "specVersion": "1.4",
        "serialNumber": "urn:uuid:" + str(uuid.uuid4()),
        "version": "1",
        "metadata": {
            "timestamp": timestamp,
        },
        "components": components,
    }

    print(json.dumps(cyclonedx, indent=2))

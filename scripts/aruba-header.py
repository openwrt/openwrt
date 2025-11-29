#!/usr/bin/python3
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright (C) 2025 Lukas Stockner <lukas@lukasstockner.de>
#
# ./aruba-header.py <input file> <output file> <build string> <version string> <oem string> <image type> <machine type>
#
# Generates a header for use with the APBoot bootloader on (some?) Aruba APs.

import argparse

LEN_BUILD = 256
LEN_VERSION = 24
LEN_OEM = 32

HEADER_MAGIC = b'ARUBA\0\0\0'


class ValidFlag:
    YES = 1


class FormatVersion:
    CURRENT = 2


class ImageType:
    ELF = 0
    FPGA_BINARY = 1
    CPBOOT_BINARY = 2
    APBOOT_BINARY = 3
    TPMINFO = 4
    APBOOT_STAGE1_BINARY = 5
    APBOOT_STAGE2_BINARY = 6
    APBOOT_COMBINED_BINARY = 7
    OS_ANCILLARY_IMAGE = 8
    XLOADER_BINARY = 9
    GRUB_BIN = 10
    LSM_PACKAGE = 11


class CompressionType:
    NONE = 0
    BZIP2 = 1
    GZIP = 2


MACHINE_TYPES = {
    'MSWITCH': 0,
    'CABERNET': 1,
    'SYRAH': 2,
    'MERLOT': 3,
    'MUSCAT': 4,
    'NEBBIOLO': 5,
    'MALBEC': 6,
    'PALOMINO': 7,
    'GRENACHE': 8,
    'MOSCATO': 9,
    'TALISKER': 10,
    'JURA_R': 11,
    'SCAPA': 12,
    'JURA_O': 13,
    'CORVINA': 14,
    'ARRAN': 15,
    'BLUEBLOOD': 16,
    'MSR2K': 17,
    'PORFIDIO': 18,
    'CAZULO': 19,
    'SCAPA_H': 20,
    'CARDHU': 21,
    'BOWMORE': 22,
    'TAMDHU': 23,
    'ARDBEG': 24,
    'ARDMORE': 25,
    'DALMORE': 26,
    'K2': 27,
    'GRAPPA': 28,
    'SHUMWAY': 29,
    'SPRINGBANK': 30,
    'OUZO': 31,
    'AMARULA': 32,
    'GROZDOVA': 33,
    'PALINKA': 34,
    'HAZELBURN': 35,
    'TOMATIN': 36,
    'HAZELBURN_H': 37,
    'TOMATIN_16': 38,
    'SPRINGBANK_16': 39,
    'OCTOMORE': 40,
    'BALVENIE': 41,
    'OUZO_PLUS': 42,
    'X4': 43,
    'EINAR': 44,
    'GLENFARCLAS': 45,
    'GLENFIDDICH': 46,
    'EIGER': 47,
    'GLENMORANGIE': 48,
    'MILAGRO': 49,
    'OPUSONE': 50,
    'ABERLOUR': 51,
    'MILLSTONE': 52,
    'DEWARS': 53,
    'BUNKER': 54,
    'MASTERSON': 55,
    'SIERRA': 56,
    'KILCHOMAN': 57,
    'SPEYBURN': 58,
    'LAGAVULIN': 59,
    'LAPHROAIG': 60,
    'TOBA': 61,
    'ARRANTA': 62,
}


class NextHeader:
    NONE = 0x00000000
    SIGN = 0x01111111


class Flags:
    C_TEST_BUILD = 0x00000001
    SWATCH = 0x00000002
    # Preserves the image with "clear all"
    DONT_CLEAR_ON_PURGE = 0x00000004
    SECURE_BOOTLOADER = 0x00000008
    FACTORY_IMAGE = 0x00000010
    FIPS_CERTIFIED = 0x00000020


def make_header(data: bytes, build: str, version: str, oem: str, imageType: int, machine: int) -> bytes:
    buildBytes = build.encode(encoding='ascii')
    assert len(buildBytes) < LEN_BUILD
    buildBytes += b'\0' * (LEN_BUILD - len(buildBytes))

    versionBytes = version.encode(encoding='ascii')
    assert len(versionBytes) < LEN_VERSION
    versionBytes += b'\0' * (LEN_VERSION - len(versionBytes))

    oemBytes = oem.encode(encoding='ascii')
    assert len(oemBytes) < LEN_OEM
    oemBytes += b'\0' * (LEN_OEM - len(oemBytes))

    header = b''
    # Payload size, image plus optional signature (which we don't use)
    header += len(data).to_bytes(4, 'big')
    # Use what appears to be the current version
    header += FormatVersion.CURRENT.to_bytes(4, 'big')
    # Checksum is computed later
    header += b'\0\0\0\0'
    # Vendor magic number
    header += HEADER_MAGIC
    # Long build information string
    header += buildBytes
    # Short version information string
    header += versionBytes
    # Image is valid
    header += ValidFlag.YES.to_bytes(1)
    # Image type
    header += imageType.to_bytes(1)
    # APBoot doesn't appear to actually support compression
    header += CompressionType.NONE.to_bytes(1)
    # Machine type
    header += machine.to_bytes(1)
    # Image size
    header += len(data).to_bytes(4, 'big')
    # Next header (we don't support signing)
    header += NextHeader.NONE.to_bytes(4, 'big')
    # MD5 checksum plus fudge factor (to ensure non-zero hash), appears unused
    header += b'\0' * 16
    header += b'\0' * 4
    # No flags are set
    header += int(0).to_bytes(4, 'big')
    # No next header is used
    header += b'\0' * 12
    # Padding
    header += b'\0' * 36
    # OEM string
    header += oemBytes
    # Padding
    header += b'\0' * 96

    assert len(header) == 512
    assert len(data) % 4 == 0

    # Compute checksum such that the big-endian sum of all 32-bit integers becomes zero.
    curSum = sum(int.from_bytes(header[i : i + 4], 'big') for i in range(0, 512, 4))
    curSum += sum(int.from_bytes(data[i : i + 4], 'big') for i in range(0, len(data), 4))

    # Set checksum
    checksum = 0x100000000 - (curSum % 0x100000000)
    header = header[:8] + checksum.to_bytes(4, 'big') + header[12:]

    return header


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Generate Aruba header.')
    parser.add_argument('source', type=argparse.FileType('r+b'))
    parser.add_argument('dest', type=argparse.FileType('wb'))
    parser.add_argument('build')
    parser.add_argument('version')
    parser.add_argument('oem')
    parser.add_argument('type', choices=['os', 'boot'])
    parser.add_argument('machine', choices=MACHINE_TYPES.keys(), type=str.upper)
    args = parser.parse_args()

    # Parse image type.
    # The OS image must be type "ELF" even though it's not an ELF file...
    imageType = {'os': ImageType.ELF, 'boot': ImageType.APBOOT_BINARY}[args.type]
    # Parse machine type.
    machineType = MACHINE_TYPES[args.machine]

    image = args.source.read()
    # Pad image.
    if len(image) % 4 != 0:
        image += b'\0' * (4 - len(image) % 4)

    # Generate header.
    header = make_header(image, args.build, args.version, args.oem, imageType, machineType)

    # Write output.
    args.dest.write(header)
    args.dest.write(image)

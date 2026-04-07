#!/usr/bin/env python3

"""
Whole Flash Image Tag

{
	u32 crc32;
	u32 version;
	u32 chipID;
	u32 flashType;
	u32 flags;
}

CRC32: Ethernet (Poly 0x04C11DB7)

Version:
	0x00005700: Any version
	0x00005731: NAND 1MB data partition
	0x00005732: Normal version

Chip ID:
	Broadcom Chip ID
	0x00006328: BCM6328
	0x00006362: BCM6362
	0x00006368: BCM6368
	0x00063268: BCM63268

Flash Type:
	1: NOR
	2: NAND 16k blocks
	3: NAND 128k blocks
	4: NAND 256k blocks
	5: NAND 512k blocks
	6: NAND 1MB blocks
	7: NAND 2MB blocks

Flags:
	0x00000001: PMC
	0x00000002: Secure BootROM

"""

import argparse
import os
import shutil
import struct
import binascii


def auto_int(x):
    return int(x, 0)


def create_tag(args, crc):
    # JAM CRC32 is bitwise not and unsigned
    crc = ~crc & 0xFFFFFFFF
    tag = struct.pack(
        ">IIIII",
        crc,
        args.tag_version,
        args.chip_id,
        args.flash_type,
        args.flags,
    )
    return tag


def create_output(args):
    # If input and output are different, copy the file first.
    # Otherwise, we can just read and append in place.
    if os.path.abspath(args.input_file) != os.path.abspath(args.output_file):
        shutil.copyfile(args.input_file, args.output_file)

    crc = 0
    # Optimization: read the file in 64KB chunks to calculate the CRC32
    # avoiding loading the entire file into memory and reducing memory complexity to O(1).
    with open(args.output_file, "r+b") as f:
        while True:
            chunk = f.read(65536)
            if not chunk:
                break
            crc = binascii.crc32(chunk, crc)

        tag = create_tag(args, crc)
        f.write(tag)


def main():
    global args

    parser = argparse.ArgumentParser(description="")

    parser.add_argument(
        "--input-file",
        dest="input_file",
        action="store",
        type=str,
        help="Input file",
    )

    parser.add_argument(
        "--output-file",
        dest="output_file",
        action="store",
        type=str,
        help="Output file",
    )

    parser.add_argument(
        "--version",
        dest="tag_version",
        action="store",
        type=auto_int,
        help="WFI Tag Version",
    )

    parser.add_argument(
        "--chip-id",
        dest="chip_id",
        action="store",
        type=auto_int,
        help="WFI Chip ID",
    )

    parser.add_argument(
        "--flash-type",
        dest="flash_type",
        action="store",
        type=auto_int,
        help="WFI Flash Type",
    )

    parser.add_argument(
        "--flags", dest="flags", action="store", type=auto_int, help="WFI Flags"
    )

    args = parser.parse_args()

    if not args.flags:
        args.flags = 0

    if (
        (not args.input_file)
        or (not args.output_file)
        or (not args.tag_version)
        or (not args.chip_id)
        or (not args.flash_type)
    ):
        parser.print_help()
    else:
        create_output(args)


main()

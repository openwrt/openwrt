#!/usr/bin/env python3

"""
CFE Partition Tag

{
	u32 part_id;
	u32 part_size;
	u16 flags;
	char part_name[33];
	char part_version[21];
	u32 part_crc32;
}

"""

import argparse
import os
import struct
import binascii


PART_NAME_SIZE = 33
PART_VERSION_SIZE = 21


def auto_int(x):
    return int(x, 0)


def create_tag(args, crc, size):
    # JAM CRC32 is bitwise not and unsigned
    crc = ~crc & 0xFFFFFFFF

    # Optimization: encode properly
    # name
    b_name = args.part_name.encode()
    if len(b_name) >= PART_NAME_SIZE:
        b_name = b_name[:PART_NAME_SIZE-1] + b'\x00'

    # version
    b_version = args.part_version.encode()
    if len(b_version) >= PART_VERSION_SIZE:
        b_version = b_version[:PART_VERSION_SIZE-1] + b'\x00'

    # struct.pack automatically pads with null bytes when using string length markers like '33s'
    return bytearray(struct.pack(
        f">IIH{PART_NAME_SIZE}s{PART_VERSION_SIZE}sI",
        args.part_id,
        size,
        args.part_flags,
        b_name,
        b_version,
        crc
    ))


def create_output(args):
    in_st = os.stat(args.input_file)
    in_size = in_st.st_size

    # Optimization: compute CRC32 in 64K chunks instead of loading entire file into memory
    # This prevents O(N) memory allocation and improves performance for large files.
    crc = 0
    with open(args.input_file, "rb") as in_f:
        while True:
            chunk = in_f.read(65536)
            if not chunk:
                break
            crc = binascii.crc32(chunk, crc)

    tag = create_tag(args, crc, in_size)

    with open(args.output_file, "wb") as out_f:
        out_f.write(tag)


def main():
    global args

    parser = argparse.ArgumentParser(description="")

    parser.add_argument(
        "--flags",
        dest="part_flags",
        action="store",
        type=auto_int,
        help="Partition Flags",
    )

    parser.add_argument(
        "--id",
        dest="part_id",
        action="store",
        type=auto_int,
        help="Partition ID",
    )

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
        "--name",
        dest="part_name",
        action="store",
        type=str,
        help="Partition Name",
    )

    parser.add_argument(
        "--version",
        dest="part_version",
        action="store",
        type=str,
        help="Partition Version",
    )

    args = parser.parse_args()

    if (
        (not args.part_flags)
        or (not args.part_id)
        or (not args.input_file)
        or (not args.output_file)
        or (not args.part_name)
        or (not args.part_version)
    ):
        parser.print_help()
    else:
        create_output(args)


main()

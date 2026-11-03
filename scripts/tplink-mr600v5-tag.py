#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Prepend the 512-byte tag that the stock U-Boot of the TP-Link Archer
MR600 v5 expects in front of an LZMA (lzma-alone) kernel payload.

U-Boot checks the first word (0x03000003) and takes the load address
(0x68), entry point (0x6c) and payload length (0x78) from the tag, all
little endian. It does not check any checksum on this path.
"""
import argparse
import struct
import sys

TAG_SIZE = 0x200
MAGIC = 0x03000003
# largest size tested; U-Boot reads at most 8 MiB of the file
MAX_SIZE = 0x600000


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--load", required=True, type=lambda x: int(x, 0))
    ap.add_argument("--entry", required=True, type=lambda x: int(x, 0))
    ap.add_argument("input")
    ap.add_argument("output")
    a = ap.parse_args()

    with open(a.input, "rb") as f:
        payload = f.read()
    # lzma-alone header: properties byte, dictionary size, 64-bit size
    if len(payload) < 13 or payload[0] >= 225:
        sys.exit("input is not an LZMA (lzma-alone) stream")

    total = TAG_SIZE + len(payload)
    if total > MAX_SIZE:
        sys.exit("tagged kernel is %d bytes, the limit is %d"
                 % (total, MAX_SIZE))

    tag = bytearray(TAG_SIZE)
    struct.pack_into("<I", tag, 0x00, MAGIC)
    struct.pack_into("<I", tag, 0x68, a.load)
    struct.pack_into("<I", tag, 0x6c, a.entry)
    struct.pack_into("<I", tag, 0x70, len(payload))
    struct.pack_into("<I", tag, 0x78, len(payload))

    with open(a.output, "wb") as f:
        f.write(tag)
        f.write(payload)


if __name__ == "__main__":
    main()

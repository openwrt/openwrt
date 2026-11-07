#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-or-later
"""Append the Buffalo WSR-1800AX4S bootcode trailer to a squashfs rootfs.

The stock bootcode (Buffalo Bootcode 1.04) CRC-checks each bank's
rootfs before booting: it computes the zlib CRC32 over the first
0x99adc6 bytes of the rootfs partition and compares it with the
ASCII trailer "gtkc:%08x\n" stored at offset 0x99adc6. The stock
squashfs happens to end exactly at that offset; a smaller OpenWrt
squashfs must therefore be padded up to it. The padding must not
be 0xff: the gap lies inside the partition range that mtdsplit
exposes as the "rootfs_data" child partition, and erased-looking
content there is misdetected as a writable filesystem area.
Random padding makes every scanner leave the gap alone.

Usage: gtkc-trailer.py <input squashfs> <output image>
"""
import os
import sys
import zlib

SPAN = 0x99ADC6
TRAILER_LEN = len("gtkc:%08x\n" % 0)


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    data = open(sys.argv[1], "rb").read()
    if data[:4] != b"hsqs":
        sys.exit("gtkc-trailer: input is not a squashfs image")
    if len(data) > SPAN:
        sys.exit("gtkc-trailer: rootfs (%d bytes) exceeds bootcode span (%d)"
                 % (len(data), SPAN))
    span = data + os.urandom(SPAN - len(data))
    crc = zlib.crc32(span) & 0xFFFFFFFF
    out = span + ("gtkc:%08x\n" % crc).encode()
    assert len(out) == SPAN + TRAILER_LEN
    open(sys.argv[2], "wb").write(out)


if __name__ == "__main__":
    main()

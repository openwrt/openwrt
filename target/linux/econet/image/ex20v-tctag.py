#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
#
# ex20v-tctag: prepend a 256-byte TrendChip "55aa" boot tag to an LZMA kernel
# so the TP-Link EX20v's free bootbase (EN7528 v1.1) can locate, decompress and
# boot it from the raw "kernel" NAND slot.
#
# The EX20v bootbase does NOT read the zytrx "HDR2" format that tclinux-trx.sh
# emits; it reads a proprietary 256-byte tag whose layout was recovered from the
# stock tclinux.bin. We keep that stock tag verbatim as a template and patch
# only the four fields that must differ for our kernel:
#
#   0x6C entry      -> 0x80002000  (bootbase decompresses to kernelAddr=0x80002000
#                                   and jumps to `entry`; our LZMA payload begins
#                                   with tclinux-free-bootbase-jump's "j 0x80020000"
#                                   trampoline at 0x80002000, so entry == kernelAddr)
#   0x78 kernelLen  -> len(lzma)
#   0x7C rootfsOff  -> 0           (our squashfs+overlay live on the UBI MTD, not
#   0x80 rootfsLen  -> 0            in this image, so the tag carries no rootfs)
#
# Everything else (magic, signature, kernelAddr=0x80002000, uncompSize buffer
# hint, kernelOffset=0x200, and the CRC at 0x98) is copied from the stock tag.
# The CRC is left as the stock value: the ROM bootbase `go` path loads+boots
# without validating it (the tag CRC is checked by the vendor web-UI/flash
# utility, not the ROM). If a device test ever shows the bootbase rejecting the
# tag on CRC, replace TCTAG_TEMPLATE's 0x98 word with a computed value.
#
# Usage: ex20v-tctag.py <in-lzma> <out-file>

import struct
import sys

# Stock EX20v tclinux.bin bytes 0x00..0x200 (the 256-byte tag lives at 0x54; the
# 0x00..0x53 preamble and 0xA0..0x1FF trailer are part of the template the
# bootbase expects to precede the LZMA payload at 0x200).
# Bytes 0x00..0x200 lifted verbatim from the stock tclinux.bin. The 55aa tag
# proper starts at 0x54 (magic1); the 0x00..0x53 preamble and 0xA0.. trailer are
# part of what the bootbase expects to precede the LZMA payload at 0x200. Fields
# patched per image: entry@0x6C, kernelLen@0x78, rootfsOff@0x7C, rootfsLen@0x80.
TCTAG_TEMPLATE = bytes.fromhex(
    "03000003021cf72d09b6f509132c594d"
    "bf272622cccc2233445566778899aabb"
    "ccddeeff000000000000000000000000"
    "000000000134a14a5900000001000000"
    "00000000000000000000000000000000"
    "0000000055aa55aaf1e2d3c4e5a66a5e"  # 0x50: ..magic1 55aa55aa, signature
    "4c3d2e1faa55aa550020008090658a80"  # 0x64: magic2, kernelAddr, entry
    "0000280200020000e9cc420000006000"  # 0x70: uncompSize, kOff, kLen, rfsOff
    "0090f40000000000000000000500aa55"  # 0x80: rfsLen, .., marker
    "000800a500000000befc7191ffffffff"  # 0x90: marker, .., CRC, ..
    "ffffffffffffffffffffffffffffffff"  # 0xA0
    "ffffffffffffffffffffffffffffffff"
    "ffffffffffffffffffffffffffffffff"
    "00000000000000000000000000000000"  # 0xD0
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "ffffffffffffffffffffffffffffffff"  # 0x1D0
    "ffffffffffffffffffffffffffffffff"
    "ffffffffffffffffffffffffffffffff"
)
assert len(TCTAG_TEMPLATE) == 0x200, hex(len(TCTAG_TEMPLATE))

OFF_ENTRY = 0x6C
OFF_KLEN = 0x78
OFF_RFSOFF = 0x7C
OFF_RFSLEN = 0x80

ENTRY = 0x80002000  # == kernelAddr; hits the free-bootbase-jump trampoline


def make_tag(lzma, entry=ENTRY, rootfs_off=0, rootfs_len=0):
    tag = bytearray(TCTAG_TEMPLATE)
    struct.pack_into("<I", tag, OFF_ENTRY, entry)
    struct.pack_into("<I", tag, OFF_KLEN, len(lzma))
    struct.pack_into("<I", tag, OFF_RFSOFF, rootfs_off)
    struct.pack_into("<I", tag, OFF_RFSLEN, rootfs_len)
    return bytes(tag)


def main():
    if len(sys.argv) != 3:
        sys.exit("usage: ex20v-tctag.py <in-lzma> <out-file>")
    lzma = open(sys.argv[1], "rb").read()
    out = make_tag(lzma) + lzma
    open(sys.argv[2], "wb").write(out)


if __name__ == "__main__":
    main()

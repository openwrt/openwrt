#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-or-later
"""
ZyXEL / MitraStar (MSTC) NAND firmware tag for BCM63268 based ZyXEL CPEs
(e.g. ZyXEL VMG8324-B10A / VMG8924-B10A, CFE board id 963168VX).

Re-implementation of the vendor tool "bcmZyNandImageBuilder" (fs_kernel
image without CFE, without ROM-D/ImageDefault), verified byte-for-byte
against vendor-built images:

  0x00000  256 byte Broadcom FILE_TAG (MSTC variant, tag version "6")
  0x00100  0xff padding up to one 128 KiB erase block
  0x20000  "rootfs" payload (JFFS2 kernel + UBI for OpenWrt)

The CFE writes the tag block to the start of the selected image slot and
the payload to the following blocks. All CRCs are Broadcom CRC32
(init 0xffffffff, no final xor), stored big endian.

FILE_TAG layout (offset, length):
  tagVersion 0/4, signiture_1 4/20, signiture_2 24/14, chipId 38/6,
  boardId 44/16, bigEndian 60/2, totalImageLen 62/10, cfeAddress 72/12,
  cfeLen 84/10, rootfsAddress 94/12, rootfsLen 106/10, kernelAddress 116/12,
  kernelLen 128/10, imageSequence 138/4, externalversion 142/32,
  internalversion 174/32, imageNext 206/1, reserved 207/9,
  imageValidationToken 216/20 (image crc, rootfs crc, kernel crc),
  tagValidationToken 236/20 (crc of bytes 0..235)
"""

import argparse
import struct
import zlib

TAG_LEN = 256
TAG_BLOCK_LEN = 128 * 1024
FLASH_BASE = 0xBFC00000


def bcm_crc32(data, crc=0xFFFFFFFF):
    # zlib computes ~crc32(~init); Broadcom omits the final inversion
    return zlib.crc32(data, crc ^ 0xFFFFFFFF) ^ 0xFFFFFFFF


def put(tag, off, length, value):
    if isinstance(value, str):
        value = value.encode()
    value = value[:length]
    tag[off:off + len(value)] = value


def create_tag(args, payload):
    tag = bytearray(TAG_LEN)

    put(tag, 0, 4, "6")
    # the vendor tool first writes "MSTC_<id>_<cid>" and then overwrites
    # it with "MSTC_<id>", leaving "<cid>" behind the terminating NUL
    put(tag, 4, 20, "MSTC_%s_%s" % (args.model_id, args.custom_model_id))
    sig1 = ("MSTC_%s" % args.model_id).encode() + b"\0"
    put(tag, 4, 20, sig1)
    put(tag, 24, 14, "ver. 2.0")
    put(tag, 38, 6, args.chip_id)
    put(tag, 44, 16, args.board_id)
    put(tag, 60, 2, "1")                      # big endian
    put(tag, 62, 10, "%d" % len(payload))     # totalImageLen
    put(tag, 72, 12, "0")                     # cfeAddress
    put(tag, 84, 10, "0")                     # cfeLen
    # vendor tool: 0xbfc00000 + size of the (not included) 128 KiB CFE
    # + one tag block; ignored for NAND but kept identical
    put(tag, 94, 12, "%d" % (FLASH_BASE + 2 * TAG_BLOCK_LEN))
    put(tag, 106, 10, "%d" % len(payload))    # rootfsLen
    put(tag, 116, 12, "0")                    # kernelAddress
    put(tag, 128, 10, "0")                    # kernelLen
    # imageSequence is left empty, the CFE assigns it while flashing
    put(tag, 142, 32, args.version)           # externalversion
    put(tag, 174, 32, args.version)           # internalversion
    tag[206] = 0                              # imageNext: nothing appended

    # word 0 (image crc over totalImageLen) is verified by the CFE when an
    # image is uploaded. Word 1 (rootfs crc over rootfsLen) is verified on
    # every boot unless it is 0; OpenWrt's UBI changes after the first boot,
    # so it is zeroed by default. Word 2 (kernel crc) covers kernelLen = 0
    # bytes, whose CRC is 0xffffffff.
    crc = bcm_crc32(payload)
    rootfs_crc = crc if args.rootfs_crc else 0
    tag[216:228] = struct.pack(">III", crc, rootfs_crc, 0xFFFFFFFF)
    tag[236:240] = struct.pack(">I", bcm_crc32(bytes(tag[:236])))

    return bytes(tag)


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--input-file", required=True)
    parser.add_argument("--output-file", required=True)
    parser.add_argument("--model-id", required=True, help="e.g. 6006")
    parser.add_argument("--custom-model-id", help="defaults to --model-id")
    parser.add_argument("--chip-id", default="63268")
    parser.add_argument("--board-id", required=True, help="e.g. 963168VX")
    parser.add_argument("--version", default="OpenWrt")
    parser.add_argument("--rootfs-crc", action="store_true",
                        help="store the rootfs crc like the vendor tool "
                             "(CFE then checks it on every boot)")
    parser.add_argument("--pad-to", type=lambda x: int(x, 0),
                        help="pad the payload with 0xff to this size; the "
                             "CFE only erases the blocks it writes")
    args = parser.parse_args()

    if not args.custom_model_id:
        args.custom_model_id = args.model_id

    with open(args.input_file, "rb") as f:
        payload = f.read()

    if args.pad_to:
        if len(payload) > args.pad_to:
            raise SystemExit("payload (%d bytes) is larger than --pad-to %d"
                             % (len(payload), args.pad_to))
        payload += b"\xff" * (args.pad_to - len(payload))

    tag = create_tag(args, payload)

    with open(args.output_file, "wb") as f:
        f.write(tag)
        f.write(b"\xff" * (TAG_BLOCK_LEN - TAG_LEN))
        f.write(payload)


if __name__ == "__main__":
    main()

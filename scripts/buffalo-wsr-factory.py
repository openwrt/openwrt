#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-or-later
"""Assemble a Buffalo WSR-1800AX4S stock-format factory image.

The stock web UI firmware updater (and the stock sysupgrade behind
it) accepts an image made of a 52-byte "encode-header" followed by
a body laid out exactly like the stock firmware file:

  [encode-header 52B]
  [kernel FIT, zero padded to 4 MiB]
  [rootfs carrying the gtkc trailer at 0x99adc6]
  [0xff padding up to rootfs-relative offset 0x9a0000]
  [fwtool metadata chunk (deadc0de header + JSON + FWx0 trailer)]

The encode-header is the byte-reversed, bit-inverted form of a
logical header holding the model string "WRTM-353AX_S", a 3-digit
version, the body size and the body zlib CRC32, protected by a raw
(reflected, no final inversion) CRC32 over the encoded header.

The version field is stamped "111" (the last stock release): the
stock updater validates the model string and the CRCs, and a stock
1.10 unit was verified to accept a "111"-stamped image.

Usage: buffalo-wsr-factory.py <image-in-out> <root.gtkc>
           <supported-devices> <dist> <version> <revision> <target> <board>

The embedded metadata JSON is composed here with a plain
"supported_devices" list.  The generic metadata_json cannot be
used for this image: on DSA-migration devices it replaces
"supported_devices" with a compat-version warning string (the real
list moves to "new_supported_devices"), which the stock firmware's
fwtool - a plain string membership test - would not match, so the
stock updater would reject the image.

The input file holds the kernel FIT; it is rewritten in place as
the complete factory image.
"""
import json
import struct
import sys
import zlib

TAB = []
for _n in range(256):
    _c = _n
    for _ in range(8):
        _c = (_c >> 1) ^ (0xEDB88320 if _c & 1 else 0)
    TAB.append(_c)


def raw_crc32(data: bytes, init: int = 0xFFFFFFFF) -> int:
    """Reflected CRC32, init 0xFFFFFFFF, no final inversion (fwtool style)."""
    crc = init
    for b in data:
        crc = TAB[(crc ^ b) & 0xFF] ^ (crc >> 8)
    return crc


KERNEL_SIZE = 0x400000
ROOT_META_OFF = 0x9A0000
STOCK_VERSION = b"111"


def build_body(kernel_fit: bytes, rootfs_seg: bytes, metadata: bytes) -> bytes:
    if len(kernel_fit) > KERNEL_SIZE:
        raise ValueError("kernel FIT exceeds 4 MiB")
    body = bytearray()
    body += kernel_fit
    body += b"\x00" * (KERNEL_SIZE - len(kernel_fit))
    if len(rootfs_seg) > ROOT_META_OFF:
        raise ValueError("rootfs segment exceeds 0x9a0000")
    body += rootfs_seg
    body += b"\xff" * (KERNEL_SIZE + ROOT_META_OFF - len(body))
    # fwtool metadata chunk: deadc0de + 8 zero bytes + JSON + FWx0 trailer
    body += b"\xde\xad\xc0\xde" + b"\x00" * 8 + metadata
    crc = raw_crc32(bytes(body))
    size = 8 + len(metadata) + 16
    body += b"FWx0" + struct.pack(">I", crc) + b"\x01\x00\x00\x00" + struct.pack(">I", size)
    return bytes(body)


def encode_header(body: bytes, version: bytes) -> bytes:
    logical = bytearray(52)
    logical[4:12] = b"FIRMWARE"
    logical[12:16] = b"520\x00"
    logical[16:19] = version
    logical[20:32] = b"WRTM-353AX_S"
    struct.pack_into("<I", logical, 44, len(body))
    struct.pack_into("<I", logical, 48, zlib.crc32(body) & 0xFFFFFFFF)
    enc_rest = bytes(b ^ 0xFF for b in reversed(bytes(logical[4:52])))
    struct.pack_into(">I", logical, 0, raw_crc32(enc_rest))
    return bytes(b ^ 0xFF for b in reversed(bytes(logical)))


def main() -> None:
    img_path, root_path = sys.argv[1:3]
    supported, dist, version, revision, target, board = sys.argv[3:9]
    metadata = json.dumps({
        "supported_devices": supported.split(),
        "version": {
            "dist": dist,
            "version": version,
            "revision": revision,
            "target": target,
            "board": board,
        },
    }).encode()
    kernel = open(img_path, "rb").read()
    root = open(root_path, "rb").read()
    body = build_body(kernel, root, metadata)
    with open(img_path, "wb") as f:
        f.write(encode_header(body, STOCK_VERSION) + body)


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""
Package Hasivo / Horaco OpenWrt images for stock Web UI firmware upgrade.

Wraps an OpenWrt uImage+SquashFS binary into the vendor Web UI upgrade format:
- Embedded reproducible tar.gz containing switch/other/do_next reboot script
- OEM MD5 hash (OEM MD5 implementation, stopping at first NUL byte)
- Appended raw firmware payload (flashed directly to RUNTIME)
- 28-byte trailer (magic 0x90ABCDEF, model, version, partition sizes, folded SHA-256)
"""

import argparse
import gzip
import hashlib
import io
import os
import struct
import sys
import tarfile
import zlib
from pathlib import Path

MAX_RUNTIME_SIZE = 0x700000  # 7 MiB RUNTIME partition limit
BLOCK_SIZE = 0x10000         # 64 KiB alignment boundary
TRAILER_MAGIC = 0x90ABCDEF


def check_image(image: bytes, expected_magic: int) -> None:
    if len(image) < 64:
        raise ValueError("Image smaller than 64-byte uImage header")
    if len(image) > MAX_RUNTIME_SIZE:
        raise ValueError(
            f"Image size ({len(image)} bytes) exceeds 7 MiB RUNTIME partition limit ({MAX_RUNTIME_SIZE} bytes)"
        )

    magic, header_crc, _timestamp, size, load, entry, data_crc = struct.unpack_from(
        ">7I", image
    )

    if magic != expected_magic:
        raise ValueError(
            f"uImage magic mismatch: 0x{magic:08x} (expected 0x{expected_magic:08x})"
        )

    if load != 0x80100000 or entry != 0x80100000:
        raise ValueError(
            f"Unexpected kernel load (0x{load:08x}) or entry (0x{entry:08x}) address (expected 0x80100000)"
        )

    header = bytearray(image[:64])
    header[4:8] = b"\0" * 4
    if header_crc != (zlib.crc32(header) & 0xFFFFFFFF):
        raise ValueError("Invalid uImage header checksum")

    if size < 1 or 64 + size > len(image):
        raise ValueError("uImage kernel extends past image boundary")

    if data_crc != (zlib.crc32(image[64 : 64 + size]) & 0xFFFFFFFF):
        raise ValueError("Invalid uImage kernel data checksum")

    if image[28:32] != b"\x05\x05\x02\x03":
        raise ValueError("Expected MIPS Linux LZMA-compressed kernel (type 05 05 02 03)")

    rootfs = (64 + size + BLOCK_SIZE - 1) // BLOCK_SIZE * BLOCK_SIZE
    if rootfs >= len(image) or image[rootfs : rootfs + 4] != b"hsqs":
        raise ValueError("Expected SquashFS filesystem ('hsqs') at 64 KiB boundary")


def build_web_factory(image: bytes, model: str) -> bytes:
    epoch = int(os.environ.get("SOURCE_DATE_EPOCH", "0"))

    buf = io.BytesIO()
    with gzip.GzipFile(fileobj=buf, mode="wb", filename="", mtime=epoch) as gz:
        with tarfile.open(fileobj=gz, mode="w:") as tar:
            reboot_script = b"#!/bin/sh\nreboot -f\n"
            ti = tarfile.TarInfo("switch/other/do_next")
            ti.size = len(reboot_script)
            ti.mode = 0o755
            ti.mtime = epoch
            ti.type = tarfile.REGTYPE
            tar.addfile(ti, io.BytesIO(reboot_script))
    tgz = buf.getvalue()

    # OEM MD5 implementation, stopping at first NUL byte
    md5 = hashlib.md5(tgz.split(b"\0", 1)[0]).digest()
    w12_body = tgz + md5

    w12 = len(w12_body)
    w16 = len(image)
    w20 = 0

    partial_trailer = struct.pack(
        ">I4s4sIII",
        TRAILER_MAGIC,
        model.encode("ascii"),
        b"owrt",
        w12,
        w16,
        w20,
    )

    pre = w12_body + image + partial_trailer
    h = hashlib.sha256(pre).digest()
    w24 = bytes(sum(h[i::4]) & 0xFF for i in range(4))

    return pre + w24


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Build Hasivo / Horaco Web UI Factory Upgrade Image"
    )
    parser.add_argument(
        "--model",
        required=True,
        help="4-character OEM model code (e.g. 9312)",
    )
    parser.add_argument(
        "--uimage-magic",
        required=True,
        type=lambda x: int(x, 0),
        help="Expected uImage header magic (e.g. 0x93000000)",
    )
    parser.add_argument(
        "--image",
        required=True,
        type=Path,
        help="Path to input kernel+rootfs image",
    )
    parser.add_argument(
        "--output",
        required=True,
        type=Path,
        help="Path to output factory image",
    )

    args = parser.parse_args()

    if len(args.model) != 4:
        sys.stderr.write("Error: --model must be exactly 4 characters\n")
        sys.exit(1)

    image_data = args.image.read_bytes()
    check_image(image_data, args.uimage_magic)

    factory_data = build_web_factory(image_data, args.model)
    args.output.write_bytes(factory_data)


if __name__ == "__main__":
    main()

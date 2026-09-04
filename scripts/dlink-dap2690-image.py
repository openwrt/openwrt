#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only

"""Create a D-Link V3 firmware image for the DAP-2690 A1."""

import argparse
import hashlib
import lzma
import struct
from pathlib import Path


HEADER_SIZE = 0xa0
KERNEL_SIZE = 0x400000
FIRMWARE_SIZE = 0xf40000
MAGIC = 0x20080321
PACK_TAG = b"--PaCkImGs--"


def fixed_string(value, size, name):
    encoded = value.encode("ascii")
    if len(encoded) >= size:
        raise ValueError(f"{name} must be shorter than {size} bytes")
    return encoded.ljust(size, b"\0")


def squashfs_data(path):
    data = path.read_bytes()
    if len(data) < 96 or data[:4] != b"hsqs":
        raise ValueError("rootfs is not a little-endian SquashFS image")

    bytes_used = struct.unpack_from("<Q", data, 40)[0]
    if bytes_used < 96 or bytes_used > len(data):
        raise ValueError("invalid SquashFS bytes_used value")
    return data[:bytes_used]


def lzma_compress(data):
    filters = [{
        "id": lzma.FILTER_LZMA1,
        "dict_size": 8 << 20,
        "lc": 3,
        "lp": 0,
        "pb": 2,
        "mode": lzma.MODE_NORMAL,
        "nice_len": 64,
        "mf": lzma.MF_BT4,
    }]
    result = bytearray(lzma.compress(data, format=lzma.FORMAT_ALONE,
                                     filters=filters))

    # The vendor encoder stores the exact uncompressed size in the LZMA-alone
    # header, whereas Python uses the streaming "unknown size" marker.
    result[5:13] = struct.pack("<Q", len(data))
    return bytes(result)


def build_image(args):
    kernel = lzma_compress(args.kernel.read_bytes())
    rootfs = squashfs_data(args.rootfs)
    if len(kernel) > KERNEL_SIZE:
        raise ValueError(
            f"compressed kernel exceeds 0x{KERNEL_SIZE:x} bytes"
        )

    tag = struct.pack(">16sI12s", PACK_TAG, len(rootfs), b"")
    payload = kernel.ljust(KERNEL_SIZE, b"\0") + tag + rootfs
    if HEADER_SIZE + len(payload) > FIRMWARE_SIZE:
        raise ValueError("firmware image exceeds the flash partition")

    offset = struct.pack(">I", 0)
    device = fixed_string(args.device, 32, "device")
    digest = hashlib.md5(offset + device + payload).digest()

    image = b"".join([
        fixed_string(args.signature, 32, "signature"),
        struct.pack("<I", MAGIC),
        struct.pack("<I", MAGIC),
        fixed_string(args.version, 16, "version"),
        fixed_string(args.model, 16, "model"),
        struct.pack("<IIII", 0, 0, 0, 0),
        fixed_string(args.build, 16, "build"),
        struct.pack(">I", len(payload)),
        offset,
        device,
        digest,
        payload,
    ])
    if len(image) != HEADER_SIZE + len(payload):
        raise ValueError("unexpected D-Link V3 header size")
    args.output.write_bytes(image)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--kernel", required=True, type=Path)
    parser.add_argument("--rootfs", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--signature", default="wapnd06_dkbs_dap2690")
    parser.add_argument("--version", default="OpenWrt")
    parser.add_argument("--model", default="dap2690")
    parser.add_argument("--build", default="snapshot")
    parser.add_argument("--device", default="/dev/mtdblock/1")
    args = parser.parse_args()

    try:
        build_image(args)
    except (OSError, ValueError, lzma.LZMAError) as error:
        parser.error(str(error))


if __name__ == "__main__":
    main()

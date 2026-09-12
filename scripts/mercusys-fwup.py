#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
#
# Wrap an OpenWrt UBI image in a Mercusys "fwup" container that the stock
# web UI (nvrammanager) accepts via firmware upgrade.
#
# The stock loader selects its verification algorithm from the `fw-type`
# string in the header: only "Cloud" maps to RSA-2048; any other value falls
# back to an MD5-only integrity check (salt-seeded, keyless). We use that
# fallback, so no vendor signing key is required. Structure and MD5 range were
# reverse-engineered from /usr/bin/nvrammanager.
#
# Header (0x1014 bytes):
#   [0x000] BE u32 file_len   [0x004..0x13] MD5   [0x014] "fw-type:<name>"
# Payload (@0x1014): interleaved 44-byte records {name[32], base BE, next_off
#   BE, size BE}, data inline after each record; rootfs UBI appended after the
#   last record.
# MD5: over file[4:] with [0x04:0x14] set to the salt, stored back into
#   [0x04:0x14].

import argparse
import hashlib
import struct
import sys

MD5_SALT = bytes.fromhex("7a2b15ed9b98596de504ab44ac2a9f4e")
HDR_LEN, REC_LEN = 0x1014, 0x2c
MIN_LEN, MAX_LEN = 0x1814, 0x3701814


def unescape(text):
    """Turn the literal \\r\\n / \\n escapes used in image Makefiles into
    real line breaks, dropping the line-continuation padding around them."""
    return text.replace("\\r\\n ", "\\r\\n").replace("\\r\\n", "\r\n") \
               .replace("\\n ", "\\n").replace("\\n", "\n")


def record(name, base, next_off, size):
    return name.ljust(0x20, b"\0") + struct.pack(">III", base, next_off, size)


def build(rootfs, fw_type, sections):
    payload = b""
    for i, (name, data) in enumerate(sections):
        base = len(payload) + REC_LEN
        next_off = 0 if i == len(sections) - 1 else base + len(data)
        payload += record(name, base, next_off, len(data)) + data
    payload += rootfs

    img = bytearray(HDR_LEN) + payload
    img[0x14:0x14 + 8 + len(fw_type)] = b"fw-type:" + fw_type
    struct.pack_into(">I", img, 0, len(img))
    img[0x04:0x14] = MD5_SALT
    img[0x04:0x14] = hashlib.md5(bytes(img[4:])).digest()
    return bytes(img)


def main():
    ap = argparse.ArgumentParser(
        description="Create a Mercusys fwup image from an OpenWrt UBI image")
    ap.add_argument("rootfs", help="UBI image holding kernel and rootfs")
    ap.add_argument("-o", "--out", required=True, help="output image")
    ap.add_argument("--fw-type", default="MR80X",
                    help='fw-type string; anything but "Cloud" skips RSA')
    ap.add_argument("--support", required=True,
                    help="contents of the support-list section")
    ap.add_argument("--soft-ver", default="2.0.0 Build OpenWrt",
                    help="version reported in the soft-version section")
    a = ap.parse_args()

    if len(a.fw_type) > 0x20:
        sys.exit("fwup: fw-type too long")

    sections = [
        (b"support-list", unescape(a.support).encode()),
        (b"soft-version", ("soft_ver:%s\n\n" % a.soft_ver).encode()),
    ]

    with open(a.rootfs, "rb") as f:
        rootfs = f.read()

    img = build(rootfs, a.fw_type.encode(), sections)
    if not MIN_LEN <= len(img) <= MAX_LEN:
        sys.exit("fwup: image length %d out of accepted range" % len(img))

    with open(a.out, "wb") as f:
        f.write(img)


if __name__ == "__main__":
    main()

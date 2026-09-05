#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Wrap an OpenWrt rootfs UBI into a TP-Link RE700X web-UI-flashable factory
# image. The image is accepted by the stock RE700X "nvrammanager" (the stock
# "Firmware Upgrade" web page), so OpenWrt can be installed without UART.
#
# This is the same TP-Link "tplink2022" container family as
# scripts/tplink-mkimage-2022.py (identical 16-byte salt, 0x1014 table /
# 0x1814 data layout, MD5(salt + image[0x14:]) header checksum), but with the
# RE700X "FwUpTbl" table variant: a 12-byte table header (rootfs-size, count,
# 0) followed by 0x2c-byte entries (name[32], offset, size, type). The rootfs
# UBI occupies the data region first (no table entry) and is flashed with
# "ubiformat -o 0x1814 -S <rootfs-size>" into the inactive dual-boot slot.
#
# The support-list and (anti-downgrade-bumped) soft-version are the generic
# RE700X EU/region metadata recovered from stock firmware; no per-device data.

import argparse
import hashlib
import struct

SALT = bytes([0x7a, 0x2b, 0x15, 0xed, 0x9b, 0x98, 0x59, 0x6d,
              0xe5, 0x04, 0xab, 0x44, 0xac, 0x2a, 0x9f, 0x4e])

TABLE_OFF = 0x1014
DATA_OFF = 0x1814          # TABLE_OFF + 0x800
ENTRY_SZ = 0x2c
MAX_ENT = 32
TABLE_SZ = 12 + MAX_ENT * ENTRY_SZ      # 0x58c, fixed memcpy size in nm_readFwUpTbl

SUPPORT_LIST = (
    "SupportList:\n"
    "{product_name:RE700X,product_ver:1.0.0,special_id:00000000}\n"
    "{product_name:RE700X,product_ver:1.0.0,special_id:4A500000}\n"
    "{product_name:RE700X,product_ver:1.0.0,special_id:554B0000}\n"
    "{product_name:RE700X,product_ver:1.0.0,special_id:45550000}\n"
    "{product_name:RE700X,product_ver:1.0.0,special_id:41550000}\n"
    "{product_name:RE700X,product_ver:1.0.0,special_id:41530000}\n"
).encode()

# soft_ver bumped to 9.9.9 so the image always passes the stock anti-downgrade
# check; fw_id / cfg_ver are the stock RE700X strings.
SOFT_VERSION = (
    "soft_ver:9.9.9 Build 20991231 Rel. 99999\n"
    "fw_id:\n"
    "cfg_ver:RE700Xh1.0.0V1.1.4P1-9CF02F8DEC4552218A6D58A4B9F5D8F1\n"
).encode()


def make_entry(name, base, size, field):
    nm = name.encode()
    if len(nm) >= 32:
        raise ValueError("section name too long: %r" % name)
    return nm + b'\x00' * (32 - len(nm)) + struct.pack('>III', base, size, field)


def build_image(os_data, meta):
    field0 = len(os_data)                       # rootfs size (ubiformat -S)
    blob = bytearray(os_data)
    bases, cur = [], field0
    for s in meta:
        bases.append(cur)
        blob += s['data']
        cur += len(s['data'])

    tbl = struct.pack('>III', field0, len(meta), 0)
    for i, s in enumerate(meta):
        tbl += make_entry(s['name'], bases[i], len(s['data']), s.get('field', 0))
    tbl += b'\x00' * (TABLE_SZ - len(tbl))

    img = bytearray(b'\xff' * (DATA_OFF + len(blob)))
    img[TABLE_OFF:TABLE_OFF + TABLE_SZ] = tbl
    img[DATA_OFF:DATA_OFF + len(blob)] = blob
    struct.pack_into('>I', img, 0, len(img))
    img[4:0x14] = hashlib.md5(SALT + bytes(img[0x14:])).digest()
    return bytes(img)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--rootfs', required=True, help='OpenWrt rootfs UBI image')
    ap.add_argument('-o', '--output', required=True, help='factory image output')
    args = ap.parse_args()

    with open(args.rootfs, 'rb') as f:
        os_data = f.read()
    meta = [
        {'name': 'support-list', 'data': SUPPORT_LIST, 'field': 0},
        {'name': 'soft-version', 'data': SOFT_VERSION, 'field': 0},
    ]
    with open(args.output, 'wb') as f:
        f.write(build_image(os_data, meta))


if __name__ == '__main__':
    main()

#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Wrap an OpenWrt rootfs UBI into a TP-Link RE70x-series web-UI-flashable
# factory image. The image is accepted by the stock "nvrammanager" (the stock
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
# RE700X and RE705X are the same design sold in different regions and share
# this container and salt. Their stock "nvrammanager" builds differ in the
# table size (RE700X: 32 slots, RE705X: 40) and the entry type they accept
# (RE700X: 0, RE705X: 1).
#
# The per-model metadata (support-list, soft-version, FwUpTbl size and entry
# type) is recovered from stock firmware. The soft-version is bumped to 9.9.9
# so the image always passes the stock anti-downgrade check. No per-device
# data.

import argparse
import hashlib
import struct

SALT = bytes([0x7a, 0x2b, 0x15, 0xed, 0x9b, 0x98, 0x59, 0x6d,
              0xe5, 0x04, 0xab, 0x44, 0xac, 0x2a, 0x9f, 0x4e])

TABLE_OFF = 0x1014
DATA_OFF = 0x1814          # TABLE_OFF + 0x800
ENTRY_SZ = 0x2c

# Per-model stock metadata. max_ent is the table size nm_readFwUpTbl reads
# (zero-filled), entry_type the type of the metadata entries. special_ids are
# the region codes the stock web UI matches against (ASCII of the first two
# bytes): RE700X EU/UK/JP/AU/AS, RE705X US/CA/TW/KR. cfg_ver is the stock
# config-version string.
PROFILES = {
    "RE700X": {
        "special_ids": ("00000000", "4A500000", "554B0000",
                        "45550000", "41550000", "41530000"),
        "cfg_ver": "RE700Xh1.0.0V1.1.4P1-9CF02F8DEC4552218A6D58A4B9F5D8F1",
        "max_ent": 32,
        "entry_type": 0,
    },
    "RE705X": {
        "special_ids": ("00000000", "43410000", "4B520000",
                        "54570000", "55530000"),
        "cfg_ver": "RE705Xh1.0.0V1.1.9P1-71817EBD7250F2E7D77FF2E77B106F9A",
        "max_ent": 40,
        "entry_type": 1,
    },
}


def support_list(model, ids):
    body = "SupportList:\n"
    for sid in ids:
        body += "{product_name:%s,product_ver:1.0.0,special_id:%s}\n" % (model, sid)
    return body.encode()


def soft_version(cfg_ver):
    return ("soft_ver:9.9.9 Build 20991231 Rel. 99999\n"
            "fw_id:\n"
            "cfg_ver:%s\n" % cfg_ver).encode()


def make_entry(name, base, size, field):
    nm = name.encode()
    if len(nm) >= 32:
        raise ValueError("section name too long: %r" % name)
    return nm + b'\x00' * (32 - len(nm)) + struct.pack('>III', base, size, field)


def build_image(os_data, meta, max_ent):
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
    tbl += b'\x00' * (12 + max_ent * ENTRY_SZ - len(tbl))

    img = bytearray(b'\xff' * (DATA_OFF + len(blob)))
    img[TABLE_OFF:TABLE_OFF + len(tbl)] = tbl
    img[DATA_OFF:DATA_OFF + len(blob)] = blob
    struct.pack_into('>I', img, 0, len(img))
    img[4:0x14] = hashlib.md5(SALT + bytes(img[0x14:])).digest()
    return bytes(img)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--model', required=True, choices=sorted(PROFILES),
                    help='TP-Link model (selects support-list / soft-version)')
    ap.add_argument('--rootfs', required=True, help='OpenWrt rootfs UBI image')
    ap.add_argument('-o', '--output', required=True, help='factory image output')
    args = ap.parse_args()

    prof = PROFILES[args.model]
    with open(args.rootfs, 'rb') as f:
        os_data = f.read()
    meta = [
        {'name': 'support-list', 'data': support_list(args.model, prof['special_ids']),
         'field': prof['entry_type']},
        {'name': 'soft-version', 'data': soft_version(prof['cfg_ver']),
         'field': prof['entry_type']},
    ]
    with open(args.output, 'wb') as f:
        f.write(build_image(os_data, meta, prof['max_ent']))


if __name__ == '__main__':
    main()

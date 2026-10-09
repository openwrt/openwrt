#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0
"""Wrap an LZMA kernel payload in the TrendChip "free bootbase" tclinux header
that the ZTE H267A (EcoNet EN751221) stock bootloader accepts.

Format reverse-engineered from the stock kernel1 image and proven
byte-identical. 256-byte big-endian header + LZMA-alone payload; four standard
zlib CRC32 fields:
  0x78 kernel crc  = crc32(payload region 0x100 .. partition_end, incl 0xFF pad)
  0x7c rootfs len  = bytes of real rootfs data (0 if none)
  0x84 rootfs crc  = crc32(rootfs[0:rootfs_len]) (0 if none)
  0xe0 header crc  = crc32(header[0x3c:0xe0])   -- computed last
The bootbase decompresses the payload to 0x80002000; pair with the
`tclinux-free-bootbase-jump` trampoline so an OpenWrt kernel linked at 0x80020000
is reached.

This board's port lives in slot 1 (kernel1@0x1680000 + rootfs1@0x280000), with
everything from 0x1A80000 up turned into one contiguous `ubi` partition. The
install channel is the bootloader's recovery page (Reset+WPS at
power-on -> http://192.168.1.254), whose CSP writer takes its slot-1
erase/write addresses from this header's 0x88/0x90 and erases 0x80000-0x3280000,
wiping any slot-2 image -- so the flashed image is the lone valid image and the
bootbase's chooser boots it from slot 1 with NO special header state. The header
therefore carries stock-default slot values (0x98/0xa0 = stock kernel2/rootfs2
offsets, 0xf8 = 0). A slot-1 retarget (0x98/0xa0 -> slot 1, 0xf8 = -1) is only
needed for the bootbase's own slot-arbitration path, which this install channel
never exercises.
"""
import argparse, struct, zlib, sys

HDR = 0x100
# This board's own stock 256B header, captured verbatim (computed fields
# zeroed). The static fields -- flash map, version string, magic -- are this
# device's actual header bytes, not a board-agnostic template: a future board
# reusing this script for its own free-bootbase image should re-capture its
# own stock header rather than assume this one applies.
TEMPLATE_HEX = (
    "999999994444444455555555aaaaaaaa"
    "00000028000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000002710000000156312e30"
    "2e365f4359502e345437000000000000"
    "00000000000000000001000000000000"
    "003fff00000001000000000000000000"
    "00400000000000000168000000400000"
    "002800000140000001a8000000400000"   # stock: 0x98 kernel2 off = 0x1a80000
    "01e80000014000005448495320495320"   # stock: 0xa0 rootfs2 off = 0x1e80000
    "48323637412056455253494f4e000000"
    "00000000000000000000000000000000"
    "00000000000000000000000100000000"
    "00000000323031393037303530393434"
    "323700000000000000000000ffffffff"   # stock: 0xf8 imageisnew=0, 0xfc imageisupgrade=-1
)
TEMPLATE = bytes.fromhex(''.join(TEMPLATE_HEX.split()))
O_KCRC, O_FSLEN, O_PSIZE, O_FSCRC, O_HCRC = 0x78, 0x7c, 0x80, 0x84, 0xe0
HCRC_RANGE = (0x3c, 0xe0)


def crc32(b):
    return zlib.crc32(b) & 0xffffffff


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--payload', required=True, help='LZMA-alone kernel body')
    ap.add_argument('--out', required=True)
    ap.add_argument('--rootfs', help='optional rootfs image (embeds FS size/crc)')
    ap.add_argument('--part-size', type=lambda x: int(x, 0), default=0x400000)
    a = ap.parse_args()

    payload = open(a.payload, 'rb').read()
    if len(payload) > a.part_size - HDR:
        sys.exit(f"tclinux: payload {len(payload)} > partition {a.part_size}-{HDR}")

    h = bytearray(TEMPLATE)
    body = payload + b'\xff' * (a.part_size - HDR - len(payload))
    struct.pack_into('>I', h, O_PSIZE, a.part_size)
    struct.pack_into('>I', h, O_KCRC, crc32(body))

    if a.rootfs:
        rf = open(a.rootfs, 'rb').read()
        rlen = len(rf)
        while rlen > 0 and rf[rlen - 1] == 0xFF:
            rlen -= 1
        struct.pack_into('>I', h, O_FSLEN, rlen)
        struct.pack_into('>I', h, O_FSCRC, crc32(rf[:rlen]))

    struct.pack_into('>I', h, O_HCRC, crc32(bytes(h[HCRC_RANGE[0]:HCRC_RANGE[1]])))

    with open(a.out, 'wb') as f:
        f.write(bytes(h))
        f.write(body)
    sys.stderr.write(
        f"tclinux: {a.out} {HDR+len(body)}B  kcrc=0x{struct.unpack('>I',h[O_KCRC:O_KCRC+4])[0]:08x}"
        f" hcrc=0x{struct.unpack('>I',h[O_HCRC:O_HCRC+4])[0]:08x}"
        f" fslen=0x{struct.unpack('>I',h[O_FSLEN:O_FSLEN+4])[0]:x}\n")


if __name__ == '__main__':
    main()

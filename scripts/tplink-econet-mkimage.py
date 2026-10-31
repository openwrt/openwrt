#!/usr/bin/env python3
#
# Wrap a FIT image in the 512-byte TP-Link/Econet image tag so that the
# stock Airoha (Econet) U-Boot 2014.04 boots it unattended.
#
# The bootloader never looks at partition names.  Its default bootcmd is
#
#     bootcmd=flash tpimg;bootm 81800200
#
# "flash tpimg" does exactly this (lib/tp_bootlib.c,
# tp_boot_get_compressed_kernel()):
#
#   sdk_flash_read(target, MTD_OFS_KERNEL, TP_TAG_LEN);   /* 512 B tag */
#   pTag = (IMAGE_TAG *)target;
#   sdk_flash_read(target, MTD_OFS_KERNEL, pTag->kernelLen + TP_TAG_LEN);
#
# and the stock U-Boot runs on a little-endian ARM core, so it dereferences
# pTag->kernelLen natively.  The second read rewinds to the same flash
# offset, so the payload lands at CONFIG_SYS_LOAD_ADDR + 0x200 and the
# "bootm 81800200" in bootcmd points exactly at it.  Only tagVersion is
# validated (common/ecnt/cmd_flash.c); magicNum, the RSA signature slots,
# oemId and binCrc32 are not checked on this path.
#
# mktplinkfw/mktplinkfw2 cannot be used here: both htonl() the size fields,
# so a little-endian bootloader would read the byte-swapped length and try to
# pull gigabytes out of NAND.  Everything below is written little-endian.
#
# Field layout (include/tp_bootlib.h, struct IMAGE_TAG, TAG_LEN == 512):

import argparse
import binascii
import struct
import sys

TAG_LEN = 0x200
TAG_VERSION_V3 = 0x03000003

OFS = {
    'tagVersion': 0x00,
    'hardwareId': 0x04,
    'firmwareId': 0x14,
    'oemId': 0x24,
    'productId': 0x34,
    'productVer': 0x38,
    'addHver': 0x3c,
    'imageValidToken': 0x40,
    'magicNum': 0x54,
    'kernelTextAddr': 0x68,
    'kernelEntryPoint': 0x6c,
    'totalImageLen': 0x70,
    'kernelAddress': 0x74,
    'kernelLen': 0x78,
    'rootfsAddress': 0x7c,
    'rootfsLen': 0x80,
    'bootAddress': 0x84,
    'bootLen': 0x88,
    'swRevision': 0x8c,
    'platformVer': 0x90,
    'specialVer': 0x94,
    'binCrc32': 0x98,
    'reserved1': 0x9c,
    'sig': 0xd0,
    'resSig': 0x150,
    'dmVersion': 0x1d0,
    'prehookFsAddress': 0x1d4,
    'prehookFsLen': 0x1d8,
    'reserved2': 0x1dc,
}

# From the shipped XX530v V2 image (example)
DEFAULT_HARDWARE_ID = '8c72826f93074c70712e33e48822a755'
DEFAULT_FIRMWARE_ID = '22011719891011778899aabbccddeeff'
DEFAULT_OEM_ID = ''
DEFAULT_PRODUCT_ID = 0x0a487c02
DEFAULT_PRODUCT_VER = 0x00060107
DEFAULT_ADD_HVER = 0x00000000
DEFAULT_SW_REVISION = 0x55aa0004
DEFAULT_PLATFORM_VER = 0xa503010a
DEFAULT_SPECIAL_VER = 0x00000000
DEFAULT_DM_VERSION = 0x5a000002

# mkimage.h: unsigned char magicNum[MAGIC_NUM_LEN]
MAGIC_NUM = bytes([
    0x55, 0xaa, 0x55, 0xaa, 0xf1, 0xe2, 0xd3, 0xc4, 0xe5, 0xa6,
    0x6a, 0x5e, 0x4c, 0x3d, 0x2e, 0x1f, 0xaa, 0x55, 0xaa, 0x55,
])


def parse_hex_field(text, name, length):
    if text is None:
        return None
    raw = bytes.fromhex(text.strip())
    if len(raw) > length:
        raise SystemExit(f'{name} is {len(raw)} bytes, at most {length} expected')
    return raw.ljust(length, b'\x00')


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('fit', help='FIT image produced by "mkimage -f *.its"')
    ap.add_argument('--output', '-o', required=True, help='tag + FIT to write here')
    ap.add_argument('--load-addr', type=lambda x: int(x, 0), required=True,
                    help='kernel load address from the FIT (kernelTextAddr)')
    ap.add_argument('--entry-point', type=lambda x: int(x, 0), default=None,
                    help='kernel entry point (kernelEntryPoint); '
                         'defaults to --load-addr')
    ap.add_argument('--hardware-id', default=DEFAULT_HARDWARE_ID,
                    help='up to 32 hex digits (16 bytes)')
    ap.add_argument('--firmware-id', default=DEFAULT_FIRMWARE_ID,
                    help='up to 32 hex digits (16 bytes)')
    ap.add_argument('--oem-id', default=DEFAULT_OEM_ID,
                    help='up to 32 hex digits (16 bytes): the per-unit flashId and '
                         'device key. Only the vendor web UI checks it.')
    ap.add_argument('--product-id', type=lambda x: int(x, 0),
                    default=DEFAULT_PRODUCT_ID)
    ap.add_argument('--product-version', type=lambda x: int(x, 0),
                    default=DEFAULT_PRODUCT_VER)
    ap.add_argument('--additional-hw-version', type=lambda x: int(x, 0),
                    default=DEFAULT_ADD_HVER)
    ap.add_argument('--sw-revision', type=lambda x: int(x, 0),
                    default=DEFAULT_SW_REVISION)
    ap.add_argument('--platform-version', type=lambda x: int(x, 0),
                    default=DEFAULT_PLATFORM_VER)
    ap.add_argument('--special-version', type=lambda x: int(x, 0),
                    default=DEFAULT_SPECIAL_VER)
    ap.add_argument('--dm-version', type=lambda x: int(x, 0), default=DEFAULT_DM_VERSION)
    args = ap.parse_args()

    with open(args.fit, 'rb') as f:
        fit = f.read()

    if fit[:4] != b'\xd0\x0d\xfe\xed':
        raise SystemExit(f'{args.fit} is not a FIT image '
                         f'(magic {fit[:4].hex()}, expected d00dfeed)')
    totalsize = struct.unpack('>I', fit[4:8])[0]
    if totalsize > len(fit):
        raise SystemExit(f'FIT totalsize {totalsize:#x} exceeds the {len(fit)} byte file')

    entry = args.entry_point if args.entry_point is not None else args.load_addr
    hw_id = parse_hex_field(args.hardware_id, 'hardware-id', 16)
    fw_id = parse_hex_field(args.firmware_id, 'firmware-id', 16)
    oem_id = parse_hex_field(args.oem_id, 'oem-id', 16)

    # SDK calc_crc32() computes the raw CRC register (init 0xffffffff, table
    # driven, no final xor) and that register value is what lands in
    # binCrc32, i.e. the bitwise complement of zlib.crc32().  Verified
    # against the stock image: 0x222be27d.
    crc = (~binascii.crc32(fit)) & 0xffffffff

    tag = bytearray(TAG_LEN)

    def put(name, value):
        struct.pack_into('<I', tag, OFS[name], value)

    put('tagVersion', TAG_VERSION_V3)
    tag[OFS['hardwareId']:OFS['hardwareId'] + 16] = hw_id
    tag[OFS['firmwareId']:OFS['firmwareId'] + 16] = fw_id
    tag[OFS['oemId']:OFS['oemId'] + 16] = oem_id
    put('productId', args.product_id)
    put('productVer', args.product_version)
    put('addHver', args.additional_hw_version)
    tag[OFS['magicNum']:OFS['magicNum'] + 20] = MAGIC_NUM
    put('kernelTextAddr', args.load_addr)
    put('kernelEntryPoint', entry)
    put('totalImageLen', len(fit))
    put('kernelLen', len(fit))
    put('swRevision', args.sw_revision)
    put('platformVer', args.platform_version)
    put('specialVer', args.special_version)
    put('binCrc32', crc)
    put('dmVersion', args.dm_version)

    if oem_id == b'\x00' * 16:
        print('tplink-econet-mkimage: warning: oemId is all zero; the vendor '
              'web UI will refuse the image, the bootloader does not care',
              file=sys.stderr)

    with open(args.output, 'wb') as f:
        f.write(tag)
        f.write(fit)

    print(f'tplink-econet-mkimage: tag {TAG_LEN:#x} B + FIT {len(fit)} B '
          f'-> {args.output}')
    print(f'  tagVersion      {TAG_VERSION_V3:#010x}')
    print(f'  kernelTextAddr  {args.load_addr:#010x}')
    print(f'  kernelEntryPoint {entry:#010x}')
    print(f'  totalImageLen   {len(fit):#010x}')
    print(f'  kernelLen       {len(fit):#010x}')
    print(f'  binCrc32        {crc:#010x}')


if __name__ == '__main__':
    main()

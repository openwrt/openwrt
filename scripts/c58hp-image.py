#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Build the C58HP raw initial firmware region; never accesses a device.

The logical firmware excludes the protected loader block. Stock U-Boot loads
the small Multi/LZMA loader through its uncached alias; the loader relocates
to 0x81800000 before validating/decompressing the lower firmware kernel.
This is a bounded RAM-installer input, not a TP-Link web-upgrade container.
"""
import argparse
import lzma
from pathlib import Path
import struct
import zlib


LOGICAL_SIZE = 0x7c0000
LOW_SIZE = 0x6c0000
BLOCK_SIZE = 0x10000
KERNEL_LOAD = 0x80060000
LOADER_LINK = 0x81800000
LOADER_ALIAS = 0xa1800000
WORKSPACE_SIZE = 0x10000


def require(condition, message):
    if not condition:
        raise ValueError(message)


def validate_firmware(firmware):
    require(64 <= len(firmware) <= LOGICAL_SIZE, 'Firmware size is outside the logical partition')
    fields = struct.unpack('>7I4B32s', firmware[:64])
    magic, hcrc, _, size, load, entry, dcrc, os_id, arch, kind, compression, _ = fields
    require(magic == 0x4f4b4c49, 'Expected an OKLI kernel')
    header = bytearray(firmware[:64])
    header[4:8] = bytes(4)
    require(zlib.crc32(header) == hcrc, 'Kernel header CRC mismatch')
    require((os_id, arch, kind, compression) == (5, 5, 2, 3), 'Expected a Linux/MIPS LZMA kernel')
    require(load == entry == KERNEL_LOAD, 'Unexpected kernel load/entry')
    end = 64 + size
    require(size >= 13 and end <= min(len(firmware), LOW_SIZE), 'Kernel crosses the loader hole')
    compressed = firmware[64:end]
    require(zlib.crc32(compressed) == dcrc, 'Kernel data CRC mismatch')
    prop = compressed[0]
    require(prop < 225, 'Invalid LZMA properties')
    probabilities = 1846 + (768 << ((prop % 9) + (prop // 9) % 5))
    require(probabilities * 4 <= WORKSPACE_SIZE, 'LZMA probability workspace is too large')
    output_size = struct.unpack_from('<Q', compressed, 5)[0]
    require(0 < output_size <= LOADER_LINK - KERNEL_LOAD, 'Kernel output overlaps the loader')
    decoder = lzma.LZMADecompressor(format=lzma.FORMAT_ALONE, memlimit=64 << 20)
    output = decoder.decompress(compressed, max_length=LOADER_LINK - KERNEL_LOAD + 1)
    require(decoder.eof and not decoder.unused_data and len(output) == output_size,
            'Kernel decompression length or stream mismatch')
    rootfs = (end + BLOCK_SIZE - 1) & -BLOCK_SIZE
    require(rootfs + 96 <= len(firmware) and firmware[rootfs:rootfs + 4] == b'hsqs',
            'Squashfs is missing at the aligned rootfs offset')
    require(struct.unpack_from('<HH', firmware, rootfs + 28) == (4, 0), 'Expected squashfs v4')
    require(struct.unpack_from('<H', firmware, rootfs + 20)[0] == 4, 'Expected XZ squashfs')
    used = struct.unpack_from('<Q', firmware, rootfs + 40)[0]
    require(96 <= used <= len(firmware) - rootfs, 'Rootfs extends beyond the firmware')
    overlay = (rootfs + used + BLOCK_SIZE - 1) & -BLOCK_SIZE
    require(LOGICAL_SIZE - overlay >= 0x80000, 'Less than 512 KiB remains for overlay')
    require(len(firmware) <= overlay + 4, 'Unexpected metadata or data beyond the rootfs padding')
    padding = firmware[rootfs + used:min(overlay, len(firmware))]
    require(all(value in (0, 255) for value in padding), 'Unexpected non-padding data after squashfs')
    if len(firmware) > overlay:
        require(firmware[overlay:] == b'\xde\xad\xc0\xde', 'Invalid JFFS2 end marker')


def validate_loader(loader, symbol_text):
    require(0 < len(loader) < BLOCK_SIZE, 'Loader does not fit its erase block')
    symbols = {}
    for line in symbol_text.splitlines():
        fields = line.split()
        if len(fields) == 3:
            symbols[fields[2]] = int(fields[0], 16)
    require(all(name in symbols for name in ('_code_start', '_code_end', '_bss_end', 'workspace')),
            'Required loader symbols are missing')
    require(symbols['_code_start'] == LOADER_LINK, 'Unexpected loader link address')
    require(symbols['_code_start'] < symbols['_code_end'] <= symbols['_bss_end'] < symbols['workspace'],
            'Loader code, BSS and workspace overlap')
    require(symbols['workspace'] + WORKSPACE_SIZE <= 0x82000000, 'Loader workspace exceeds its RAM range')


def loader_block(loader):
    compressed = bytearray(lzma.compress(loader, format=lzma.FORMAT_ALONE,
                           filters=[{'id': lzma.FILTER_LZMA1, 'dict_size': 1 << 23,
                                     'lc': 1, 'lp': 2, 'pb': 2}]))
    compressed[5:13] = struct.pack('<Q', len(loader))
    payload = struct.pack('>II', len(compressed), 0) + compressed
    header = bytearray(struct.pack('>7I4B32s', 0x27051956, 0, 0, len(payload),
                                  LOADER_ALIAS, LOADER_ALIAS, zlib.crc32(payload),
                                  5, 5, 4, 3, b'C58HP stage2 v1'))
    header[4:8] = struct.pack('>I', zlib.crc32(header))
    image = header + payload
    require(len(image) <= BLOCK_SIZE, 'Compressed loader does not fit its erase block')
    return bytes(image).ljust(BLOCK_SIZE, b'\xff')


def build_image(firmware, loader, symbols):
    validate_firmware(firmware)
    validate_loader(loader, symbols)
    block = loader_block(loader)
    logical = firmware.ljust(LOGICAL_SIZE, b'\xff')
    return logical[:LOW_SIZE] + block + logical[LOW_SIZE:]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--firmware', type=Path, required=True)
    parser.add_argument('--loader', type=Path, required=True)
    parser.add_argument('--symbols', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    image = build_image(args.firmware.read_bytes(), args.loader.read_bytes(), args.symbols.read_text())
    temporary = args.output.with_name(args.output.name + '.new')
    temporary.write_bytes(image)
    temporary.replace(args.output)


if __name__ == '__main__':
    main()

#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Exercise C58HP image bounds and CRC rejection without a router or SDK."""
import importlib.util
import lzma
from pathlib import Path
import struct
import unittest
import zlib


spec = importlib.util.spec_from_file_location(
    'c58hp_image', Path(__file__).resolve().parents[1] / 'c58hp-image.py')
image = importlib.util.module_from_spec(spec)
spec.loader.exec_module(image)


def kernel(payload=b'kernel-and-dtb' * 128, load=image.KERNEL_LOAD):
    compressed = bytearray(lzma.compress(payload, format=lzma.FORMAT_ALONE,
        filters=[{'id': lzma.FILTER_LZMA1, 'dict_size': 1 << 20,
                  'lc': 1, 'lp': 2, 'pb': 2}]))
    compressed[5:13] = struct.pack('<Q', len(payload))
    return wrap(compressed, load)


def wrap(compressed, load=image.KERNEL_LOAD):
    header = bytearray(struct.pack('>7I4B32s', 0x4f4b4c49, 0, 0,
        len(compressed), load, load, zlib.crc32(compressed), 5, 5, 2, 3, b'test'))
    header[4:8] = struct.pack('>I', zlib.crc32(header))
    return header + compressed


def firmware(wrapped=None, rootfs_bytes=0x10123):
    if wrapped is None:
        wrapped = kernel()
    rootfs_offset = (len(wrapped) + 0xffff) & ~0xffff
    rootfs = bytearray(rootfs_bytes)
    rootfs[:4] = b'hsqs'
    struct.pack_into('<I', rootfs, 12, 1 << 20)
    struct.pack_into('<H', rootfs, 20, 4)
    struct.pack_into('<HH', rootfs, 28, 4, 0)
    struct.pack_into('<Q', rootfs, 40, rootfs_bytes)
    content = bytearray(wrapped).ljust(rootfs_offset, b'\xff') + rootfs
    return content.ljust((len(content) + 0xffff) & ~0xffff, b'\xff') + b'\xde\xad\xc0\xde'


SYMBOLS = '''81800000 T _code_start
81801200 T _code_end
81801300 B _bss_end
81802000 B workspace
'''


class ImageValidation(unittest.TestCase):
    def test_initial_region_has_exact_mapping_and_uncached_loader(self):
        logical = firmware(rootfs_bytes=0x6d0123)
        loader = b'raw-loader' * 200
        result = image.build_image(logical, loader, SYMBOLS)
        self.assertEqual(len(result), 0x7d0000)
        self.assertEqual(result[:0x6c0000] + result[0x6d0000:],
                         logical.ljust(0x7c0000, b'\xff'))
        block = result[0x6c0000:0x6d0000]
        fields = struct.unpack('>7I4B32s', block[:64])
        magic, crc, timestamp, size, load, entry, data_crc = fields[:7]
        self.assertEqual((magic, timestamp, load, entry),
                         (0x27051956, 0, 0xa1800000, 0xa1800000))
        self.assertEqual(fields[7:11], (5, 5, 4, 3))
        header = bytearray(block[:64])
        header[4:8] = bytes(4)
        self.assertEqual(zlib.crc32(header), crc)
        payload = block[64:64 + size]
        self.assertEqual(zlib.crc32(payload), data_crc)
        compressed_bytes, terminator = struct.unpack('>II', payload[:8])
        self.assertEqual((compressed_bytes, terminator), (len(payload) - 8, 0))
        self.assertEqual(lzma.decompress(payload[8:], format=lzma.FORMAT_ALONE), loader)
        self.assertEqual(block[64 + size:], b'\xff' * (0x10000 - 64 - size))

    def test_header_and_payload_corruption_are_rejected(self):
        for offset, message in [(8, 'header CRC'), (64 + 13, 'data CRC')]:
            with self.subTest(offset=offset):
                content = firmware()
                content[offset] ^= 1
                with self.assertRaisesRegex(ValueError, message):
                    image.validate_firmware(content)

    def test_wrong_kernel_address_is_rejected(self):
        with self.assertRaisesRegex(ValueError, 'load/entry'):
            image.validate_firmware(firmware(kernel(load=0x80000000)))

    def test_unbounded_or_overlapping_kernel_output_is_rejected(self):
        for size in (0, 0xffffffffffffffff, image.LOADER_LINK - image.KERNEL_LOAD + 1):
            with self.subTest(size=size):
                compressed = bytearray(kernel()[64:])
                struct.pack_into('<Q', compressed, 5, size)
                with self.assertRaisesRegex(ValueError, 'output overlaps'):
                    image.validate_firmware(firmware(wrap(compressed)))

    def test_excessive_lzma_workspace_is_rejected(self):
        compressed = bytearray(kernel()[64:])
        compressed[0] = 44  # lc=8, lp=4: too many probabilities for 64 KiB.
        with self.assertRaisesRegex(ValueError, 'workspace'):
            image.validate_firmware(firmware(wrap(compressed)))

    def test_kernel_crossing_loader_hole_is_rejected(self):
        wrapped = wrap(bytes(0x6c0000))
        with self.assertRaisesRegex(ValueError, 'loader hole'):
            image.validate_firmware(wrapped)

    def test_trailing_lzma_data_is_rejected(self):
        with self.assertRaisesRegex(ValueError, 'stream mismatch'):
            image.validate_firmware(firmware(wrap(kernel()[64:] + b'extra')))

    def test_rootfs_bounds_and_overlay_reserve_are_enforced(self):
        for used, message in [(0xffffffffff, 'beyond'), (0x740001, '512 KiB')]:
            with self.subTest(used=used):
                content = firmware(rootfs_bytes=0x740001) if used == 0x740001 else firmware()
                struct.pack_into('<Q', content, 0x10000 + 40, used)
                with self.assertRaisesRegex(ValueError, message):
                    image.validate_firmware(content)

    def test_metadata_and_nonpadding_bytes_are_rejected(self):
        with self.assertRaisesRegex(ValueError, 'metadata'):
            image.validate_firmware(firmware() + b'fwtool metadata')
        content = firmware()
        content[-5] = 42
        with self.assertRaisesRegex(ValueError, 'non-padding'):
            image.validate_firmware(content)

    def test_loader_ranges_and_missing_symbols_are_rejected(self):
        for loader, symbols, message in [
            (b'', SYMBOLS, 'erase block'),
            (bytes(0x10000), SYMBOLS, 'erase block'),
            (b'x', '', 'symbols'),
            (b'x', SYMBOLS.replace('81800000', 'a1800000'), 'link address'),
            (b'x', SYMBOLS.replace('81802000', '81801200'), 'overlap'),
            (b'x', SYMBOLS.replace('81802000', '81ff0001'), 'RAM range'),
        ]:
            with self.subTest(message=message, size=len(loader)):
                with self.assertRaisesRegex(ValueError, message):
                    image.validate_loader(loader, symbols)


if __name__ == '__main__':
    unittest.main()

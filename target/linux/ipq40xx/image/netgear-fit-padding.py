#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# -*- coding: utf-8 -*-

# NETGEAR EX6150v2 padding tool
# (c) 2024 David Bauer <mail@david-bauer.net>

import math
import sys

FLASH_BLOCK_SIZE = 64 * 1024


def read_field(data, offset):
    return data[offset + 3] | data[offset + 2] << 8 | data[offset + 1] << 16 | data[offset] << 24


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('Usage: {} <input-image> <output-image>'.format(sys.argv[0]))
        sys.exit(1)

    with open(sys.argv[1], 'rb') as f:
        data = f.read()

    file_len = len(data)

    # File-len in fdt header at offset 0x4
    file_len_hdr = read_field(data, 0x4)
    # String offset in fdt header at offset 0xc
    str_off = read_field(data, 0xc)

    print("file_len={} hdr_file_len={} str_off={}".format(file_len, file_len_hdr, str_off))

    # Off to NETGEAR calculations - Taken from u-boot source (cmd_dni.c:2145)
    #
    # rootfs_addr = (ntohl(hdr->ih_size)/CONFIG_SYS_FLASH_SECTOR_SIZE+1) * CONFIG_SYS_FLASH_SECTOR_SIZE +
    #               2*sizeof(image_header_t)-sizeof(image_header_t);
    # rootfs_addr = rootfs_addr - (0x80 - mem_addr);

    # NETGEAR did fuck up badly. The image uses a FIT header, while the calculation is done on a legacy header
    # assumption. 'ih_size' matches 'off_dt_strings' of a fdt_header.
    # From my observations, this seems to be fixed on newer bootloader versions.
    # However, we need to be compatible with both.

    # This presents a challenge: FDT_STR might end short of a block boundary, colliding with the rootfs_addr
    #
    # Our dirty solution:
    #  - Move the string_table to match a block_boundary.
    #  - Update the total file_len to end on 50% of a block boundary.
    #
    # This ensures all netgear calculations will be correct, regardless whether they are done based on the
    # 'off_dt_strings' or 'totalsize' fields of a fdt header.

    new_dt_strings = int((math.floor(file_len / FLASH_BLOCK_SIZE) + 2) * FLASH_BLOCK_SIZE)
    new_image_len = int(new_dt_strings + (FLASH_BLOCK_SIZE / 2))
    new_file_len = int(new_dt_strings + FLASH_BLOCK_SIZE - 64)
    print(f"new_file_len={new_file_len} new_hdr_file_len={new_image_len} new_str_offset={new_dt_strings}")

    # Convert data to bytearray
    data = bytearray(data)

    # Enlarge byte-array to new size
    data.extend(bytearray(new_file_len - file_len))

    # Assert that the new and old string-tables are at least 256 bytes apart.
    # We pad by two blocks, but let's be extra sure.
    assert new_dt_strings - str_off >= 256

    # Move the string table to the new offset
    for i in range(0, 256):
        data[new_dt_strings + i] = data[str_off + i]
        data[str_off + i] = 0

    # Update the string offset in the header
    data[0xc] = (new_dt_strings >> 24) & 0xFF
    data[0xd] = (new_dt_strings >> 16) & 0xFF
    data[0xe] = (new_dt_strings >> 8) & 0xFF
    data[0xf] = new_dt_strings & 0xFF

    # Update the file length in the header
    data[0x4] = (new_image_len >> 24) & 0xFF
    data[0x5] = (new_image_len >> 16) & 0xFF
    data[0x6] = (new_image_len >> 8) & 0xFF
    data[0x7] = new_image_len & 0xFF

    # Write the new file
    with open(sys.argv[1] + '.new', 'wb') as f:
        f.write(data)

#! /usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-or-later

import argparse
import struct

from binascii import crc32
from dataclasses import dataclass
from itertools import cycle
from typing import List


def xor(data: bytes) -> bytes:
    passphrase = "Seek AGREEMENT for the date of completion.\0"
    pw = cycle(bytearray(passphrase.encode('ascii')))
    return bytearray(b ^ next(pw) for b in data)


def add_fw_header(data: bytes, magic: int, hwid: int, build_id: int,
                  offsets: List[int]) -> bytes:
    unknown_1 = 0x01
    unknown_2 = 0x0000
    unknown_3 = 0x00000000
    unknown_4 = 0x01000000
    file_crc = crc(data, 0)

    header_struct = struct.Struct('>QIBBHIIIIII' + 'I' * len(offsets))
    header_size = header_struct.size
    file_size = header_size + len(data)

    header_offsets = map(lambda x: x + header_size, offsets)

    header_data = header_struct.pack(magic, file_size, unknown_1, len(offsets),
                                     unknown_2, hwid, build_id, unknown_3,
                                     build_id, unknown_4, *header_offsets,
                                     file_crc)
    return header_data + data


def add_file_header(data: bytes, filename: str, build_id: int) -> bytes:
    unknown1 = 0x01000000
    unknown2 = 0x00000000
    file_crc = crc(data, 0)

    header_struct = struct.Struct(">16sIIIII")
    file_size = header_struct.size + len(data)

    header_data = header_struct.pack(filename.encode('ascii'), file_size,
                                     unknown1, build_id, unknown2, file_crc)
    return header_data + data


def crc(data: bytes, init_val: int) -> int:
    return 0xffffffff ^ (crc32(data, 0xffffffff ^ init_val))


@dataclass
class Partition:
    name: str
    size: int


def main():
    partitions = [
        Partition(name='kernel', size=2048 * 1024),
        Partition(name='root', size=9216 * 1024),
        Partition(name='userdisk', size=3076 * 1024),
    ]

    parser = argparse.ArgumentParser(prog='moxa-encode-fw',
                                     description='MOXA IW firmware encoder')
    parser.add_argument('-i', '--input', required=True, type=str, help='Firmware file')
    parser.add_argument('-o', '--output', required=True, type=str, help="Output path for encoded firmware file")
    parser.add_argument('-m', '--magic', required=True, type=lambda x: int(x,0), help="Magic for firmware header")
    parser.add_argument('-d', '--hwid', required=True, type=lambda x: int(x,0), help="Hardware id of device")
    parser.add_argument('-b', '--buildid', required=True, type=lambda x: int(x,0), help="Build id of firmware")
    args = parser.parse_args()

    with open(args.input, 'rb') as input_file:
        firmware = bytearray(input_file.read())

    offsets = []
    pos_input = 0
    pos_output = 0
    firmware_seg = bytearray()

    for partition in partitions:
        part_data = firmware[pos_input:pos_input + partition.size]

        # just to make sure that no partition is empty
        if len(part_data) == 0:
            part_data = bytearray([0x00])

        header = add_file_header(part_data, partition.name, args.buildid)
        firmware_seg += header

        offsets.append(pos_output)
        pos_input += partition.size
        pos_output += len(header)

    moxa_firmware = add_fw_header(firmware_seg, args.magic, args.hwid, args.buildid, offsets)

    encrypted = xor(moxa_firmware)
    with open(args.output, 'wb') as output_file:
        output_file.write(encrypted)


if __name__ == '__main__':
    main()

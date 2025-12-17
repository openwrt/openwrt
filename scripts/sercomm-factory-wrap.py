#!/usr/bin/env python3

import argparse
import binascii
import codecs
import struct
import sys


PID_MAGIC = b'sErCoMm'
PID2_MAGIC = b'\x90\xf7eRcOmM\0\0'

HEADER_SIZE = 256
HEADER_MAGIC = b'Ser\0'
HEADER_MAX_IMAGES = 8
CHECKSUM_BITMAP = 0xff


def auto_int(x):
	return int(x, 0)


def pack(structure, endian='<'):
	form = ''.join([x if x != 's' else f'{len(y)}s' for x, y in structure])
	return struct.pack(endian + form, *[y for x, y in structure])


def padded(data, align, value=b'\xff'):
	length = len(data)
	aligned = (length + (align - 1)) // align * align
	return data + value * (aligned - length)


parser = argparse.ArgumentParser(description='This script wraps firmware \
	images into Sercomm compatible factory update or partition format.')

parser.add_argument('--hw-id', help='Hardware ID as hex string')
parser.add_argument('--hw-revision', type=auto_int, help='Hardware revision')
parser.add_argument('--function-code', type=auto_int,
	help='Device function code')
parser.add_argument('--company-code', type=auto_int, help='Device company code')
parser.add_argument('--fw-version', type=auto_int, default=0x0020,
	help='Firmware version in the format 0xssmn with s=sub, m=major, n=minor')
parser.add_argument('--kernel-split', type=auto_int,
	help='Max size of first image')
parser.add_argument('--no-pid', action='store_true', default=False,
	help='Do not write initial padding and PID of factory image')
parser.add_argument('--header-offset', type=auto_int,
	help='The header offset in the factory image or partition')

parser.add_argument('input')
parser.add_argument('output')

args = parser.parse_args()

with open(args.input, 'rb') as file:
	data = file.read()

images = b''
imageCount = 0
offset = base = args.header_offset + HEADER_SIZE
parts = [data[:args.kernel_split], data[args.kernel_split:]]
for part in parts:
	if not part:
		break

	images += pack([
		('I', offset),
		('I', len(part)),
		('I', binascii.crc32(part)),
		('I', 0), # version
		('Q', 0xffffffffffffffff) # reserved
	])

	imageCount += 1
	offset += len(part)

data = padded(data, 0x1000)

headerStruct = [
	('s', HEADER_MAGIC),
	('I', base + len(data)), # pid2 address
	('I', 0), # header checksum
	('B', imageCount),
	('B', CHECKSUM_BITMAP),
	('H', 0xffff) # reserved
]

headerStruct[2] = ('I', binascii.crc32(pack(headerStruct)))
header = padded(pack(headerStruct) + images, HEADER_SIZE)

pid2 = pack([
	('B', args.hw_revision),
	('B', args.function_code),
	('H', args.company_code),
	('H', args.fw_version),
	('s', PID2_MAGIC)
])

with open(args.output, 'wb') as file:
	if not args.no_pid:
		pid = pack([
			('s', PID_MAGIC),
			('I', 0), # reserved
			('34s', padded(codecs.decode(args.hw_id, 'hex'), 34, b'\0')),
			('B', args.hw_revision),
			('B', args.function_code),
			('H', args.company_code),
			('Q', 0), # reserved
			('H', args.fw_version),
			('I', 0), # reserved
			('s', PID_MAGIC)
		])

		padding = padded(b'\xff', args.header_offset - len(pid))

		file.write(padding)
		file.write(pid)

	file.write(header)
	file.write(data)
	file.write(pid2)

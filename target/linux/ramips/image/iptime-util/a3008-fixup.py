#!/usr/bin/env python3
import sys, zlib, struct
f = sys.argv[1]
d = bytearray(open(f, 'rb').read())
assert d[:4] == b'\x27\x05\x19\x56'
d[0x20:0x40] = b'a3008'.ljust(16, b'\0') + b'16_000'.ljust(16, b'\0')
d[0x3d] = 0x2e
d[4:8] = b'\0' * 4
d[4:8] = struct.pack('>I', zlib.crc32(bytes(d[:64])) & 0xffffffff)
open(f, 'wb').write(d)

#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Assemble the RTX830 SD layout; never accept a block device as output."""
import shutil
import stat
import struct
import sys
from pathlib import Path

out, boot, root = map(Path, sys.argv[1:])
if out.exists() and not stat.S_ISREG(out.stat().st_mode):
    raise SystemExit("Output must be a regular file")
if boot.stat().st_size != 64 * 1024**2 or root.stat().st_size != 512 * 1024**2:
    raise SystemExit("RTX830 requires a 64 MiB boot and 512 MiB ext4 image")
mbr = bytearray(512)
struct.pack_into("<I", mbr, 440, 0x83002026)
for offset, kind, start, count in ((446, 0x0c, 2048, 131072),
                                   (462, 0x83, 133120, 1048576)):
    struct.pack_into("<B3sB3sII", mbr, offset, 0, b"\xfe\xff\xff", kind,
                     b"\xfe\xff\xff", start, count)
mbr[510:512] = b"\x55\xaa"
with out.open("wb") as dst:
    dst.truncate(577 * 1024**2)
    dst.write(mbr)
    for src, sector in ((boot, 2048), (root, 133120)):
        dst.seek(sector * 512)
        with src.open("rb") as stream:
            shutil.copyfileobj(stream, dst)

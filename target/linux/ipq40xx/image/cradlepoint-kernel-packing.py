#!/usr/bin/env python3

import zlib
import sys
import os

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print('Usage: {} <input-gzip> <output-gzip> <input-squashfs>'.format(sys.argv[0]))
        sys.exit(1)
    
    input_gzip_path = sys.argv[1]
    output_gzip_path = sys.argv[2]
    rootfs_file_path = sys.argv[3]
    
    with (open(rootfs_file_path, 'r+b') as r):
        rootfs = r.read()
        curr_len = len(rootfs)

        # pad out squashfs image
        fs_needed_len = (4096 - (curr_len) % 0x1000)
        fs_pad = b'\x00' * fs_needed_len

        r.seek(0, os.SEEK_END)
        r.write(fs_pad)
        
        fs_len = curr_len + fs_needed_len

        # seek back to the beginning, and re-read into RAM
        r.seek(0)
        new_rootfs = r.read()
    
    fs_chksum = zlib.crc32(new_rootfs).to_bytes(4, "little")

    # footer should start on a 4-byte-aligned boundary
    kern_len = os.path.getsize(input_gzip_path)
    needed_padding = 4 - ((kern_len) % 4)
    front_padding = b'\xaa' * needed_padding
    
    footer = front_padding + b'\x00\x00\x00\x00\x27\x05\x19\x58' + fs_chksum + fs_len.to_bytes(4, "little") + b'\x00\x00\x00\x00'

    with (open(input_gzip_path, 'rb') as i):
        in_file = i.read()
    
    with (open(output_gzip_path, 'wb') as o):
        o.write(in_file)
        o.write(footer)

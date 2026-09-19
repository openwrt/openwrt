#!/usr/bin/env python3

import argparse

from ftplib import FTP
from os import stat

parser = argparse.ArgumentParser(description='Tool to boot AVM EVA ramdisk images.')
parser.add_argument('ip', type=str, help='IP-address to transfer the image to')
parser.add_argument('image', type=str, help='Location of the ramdisk image')
parser.add_argument('--offset', type=lambda x: int(x,0), help='Offset to load the image to in hex format with leading 0x. Only needed for non-lantiq devices.')
parser.add_argument('--ramsize', type=lambda x: int(x,0), help='RAM size in hex format with leading 0x.')
parser.add_argument('--ramstart', type=lambda x: int(x,0), help='RAM start address in hex format with leading 0x.')
args = parser.parse_args()

ram_size = args.ramsize if args.ramsize else 0x8000000
ram_start = args.ramstart if args.ramstart else 0x80000000

end_addr = ram_start + ram_size

size = stat(args.image).st_size
# arbitrary size limit, to prevent the address calculations from overflows etc.
assert size < 0x2000000

if args.offset:
	memsize = size
	start_addr = args.offset
else:
	# We need to align the address.
	# A page boundary seems to be sufficient on 7362sl and 7412
	memsize = ((ram_size - size) & ~0xfff)
	start_addr = ram_start + memsize

img = open(args.image, "rb")
ftp = FTP(args.ip, 'adam2', 'adam2')

def adam(cmd):
	print("> %s"%(cmd))
	resp = ftp.sendcmd(cmd)
	print("< %s"%(resp))
	assert resp[0:3] == "200"

ftp.set_pasv(True)
# The following parameters allow booting the avm recovery system with this
# script.
adam('SETENV memsize 0x%08x'%(memsize))
adam('SETENV kernel_args_tmp mtdram1=0x%08x,0x%08x'%(start_addr, end_addr))
adam('MEDIA SDRAM')
ftp.storbinary('STOR 0x%08x 0x%08x'%(start_addr, end_addr), img)
img.close()
ftp.close()

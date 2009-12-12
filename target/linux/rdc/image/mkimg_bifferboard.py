#!/usr/bin/env python

"""
   Create firmware for 8MB Bifferboards
   Firmware does not include the config blocks
   Firmware starts just after config
"""

import struct, sys

kernel_extent = 0x200000
config = 0x6000

if __name__ == "__main__":

  if len(sys.argv) != 4:
    print  "usage: mkimg_bifferboard.py <kernel> <64k JFFS> <output file>"
    sys.exit(0)
    
  bzimage = sys.argv[1]
  rootfs = sys.argv[2]
  target = sys.argv[3]

  # Kernel first
  fw = file(bzimage).read()
  if len(fw) > (kernel_extent - config):
    raise IOError("Kernel too large")

  # Pad up to 0x200000
  while len(fw) < (kernel_extent - config):
    fw += "\xff"

  fw += file(rootfs).read()

  # Check length of total
  if len(fw) > (0x800000 - 0x10000 - 0x6000):
    raise IOError("Rootfs too large")

  file(target,"wb").write(fw)
  print "Firmware written to '%s'" % target

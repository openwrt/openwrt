#!/usr/bin/env python

"""
   Create firmware for 4/8MB Bifferboards, suitable for uploading using
   either bb_upload8.py or bb_eth_upload8.py
"""

import struct, sys

# Increase the kmax value if the script gives errors about the kernel being 
# too large.  You need to set the Biffboot kmax value to the same value you
# use here.
kmax = 0x10

# No need to change this for 4MB devices, it's only used to tell you if 
# the firmware is too large!
flash_size = 0x800000

# This is always the same, for 1MB, 4MB and 8MB devices
config_extent = 0x6000

kernel_extent = kmax * 0x10000

if __name__ == "__main__":

  if len(sys.argv) != 4:
    print  "usage: mkimg_bifferboard.py <kernel> <rootfs> <output file>"
    sys.exit(-1)
    
  bzimage = sys.argv[1]
  rootfs = sys.argv[2]
  target = sys.argv[3]

  # Kernel first
  fw = file(bzimage).read()
  if len(fw) > (kernel_extent - config_extent):
    raise IOError("Kernel too large")

  # Pad up to end of kernel partition
  while len(fw) < (kernel_extent - config_extent):
    fw += "\xff"

  fw += file(rootfs).read()

  # Check length of total
  if len(fw) > (flash_size - 0x10000 - config_extent):
    raise IOError("Rootfs too large")

  file(target,"wb").write(fw)
  print "Firmware written to '%s'" % target

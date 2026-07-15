# SPDX-License-Identifier: GPL-2.0-or-later

define Device/netgear_cg3000_2staus
  $(Device/bcm33xx-hcs)
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := CG3000-2STAUS
  CHIP_ID := 3383
  BLOCKSIZE := 0x10000
  HCS_MAGIC_BYTES := 0xa0ff
  HCS_REV_MIN := 0000
  HCS_REV_MAJ := 0003
  DEVICE_PACKAGES += kmod-leds-bcm6328 bcm3380-msp-firmware
endef
TARGET_DEVICES += netgear_cg3000_2staus

# SPDX-License-Identifier: GPL-2.0-or-later

define Device/netgear_cg3000-2staus
  $(Device/bcm33xx-uboot-ubi)
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := CG3000-2STAUS
  CHIP_ID := 3383
  BLOCKSIZE := 0x20000
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  VID_HDR_OFFSET := 2048
  HCS_MAGIC_BYTES := 0xa0ff
  HCS_REV_MIN := 0000
  HCS_REV_MAJ := 0003
  DEVICE_PACKAGES += kmod-leds-bcm6328 bcm3380-msp-firmware
endef
TARGET_DEVICES += netgear_cg3000-2staus

# SPDX-License-Identifier: GPL-2.0-or-later

define Device/netgear_c6300bd-1tlaus
  $(Device/bcm33xx-uboot-ubi)
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := C6300BD-1TLAUS
  CHIP_ID := 3384
  BLOCKSIZE := 0x20000
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  VID_HDR_OFFSET := 2048
  HCS_MAGIC_BYTES := 0xa0eb
  HCS_REV_MIN := 0000
  HCS_REV_MAJ := 0003
  DEVICE_LOADADDR := 0x88010000
  DEVICE_PACKAGES += $(USB2_PACKAGES) kmod-usb-ohci
endef
TARGET_DEVICES += netgear_c6300bd-1tlaus

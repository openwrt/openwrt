# SPDX-License-Identifier: GPL-2.0-or-later

define Device/netgear_cg3100d
  $(Device/bcm33xx-hcs)
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := CG3100D
  CHIP_ID := 3380
  HCS_MAGIC_BYTES := 0xa0e3
  HCS_REV_MIN := 0000
  HCS_REV_MAJ := 0003
  DEVICE_PACKAGES += $(USB2_PACKAGES)
endef
TARGET_DEVICES += netgear_cg3100d

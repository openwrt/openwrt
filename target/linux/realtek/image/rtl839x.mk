# SPDX-License-Identifier: GPL-2.0-only

define Device/netgear_gs728tpv2
  SOC := rtl8391
  IMAGE_SIZE := 13952k
  DEVICE_VENDOR := Netgear
  DEVICE_MODEL := GS728TP
  DEVICE_VARIANT := v2
  UIMAGE_MAGIC := 0x174e4742
  DEVICE_PACKAGES += lua-rs232
  KERNEL_INITRAMFS := kernel-bin | append-dtb | gzip | uImage gzip
endef
TARGET_DEVICES += netgear_gs728tpv2

define Device/zyxel_gs1900-48
  SOC := rtl8393
  IMAGE_SIZE := 13952k
  DEVICE_VENDOR := ZyXEL
  UIMAGE_MAGIC := 0x83800000
  ZYXEL_VERS := AAHO
  DEVICE_MODEL := GS1900-48
  KERNEL_INITRAMFS := kernel-bin | append-dtb | gzip | zyxel-vers $$$$(ZYXEL_VERS) | \
	uImage gzip
endef
TARGET_DEVICES += zyxel_gs1900-48

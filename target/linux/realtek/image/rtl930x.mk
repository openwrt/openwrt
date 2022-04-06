# SPDX-License-Identifier: GPL-2.0-only

define Device/ubiquiti_usw-aggregation
  SOC := rtl9303
  IMAGE_SIZE := 13504k
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := USW-Aggregation
endef
TARGET_DEVICES += ubiquiti_usw-aggregation

define Device/zyxel_xgs1250-12
  SOC := rtl9302
  UIMAGE_MAGIC := 0x93001250
  ZYXEL_VERS := ABWE
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := XGS1250-12
    IMAGE_SIZE := 13312k
  KERNEL_INITRAMFS := kernel-bin | append-dtb | gzip | zyxel-vers $$$$(ZYXEL_VERS) | \
	uImage gzip
endef

TARGET_DEVICES += zyxel_xgs1250-12

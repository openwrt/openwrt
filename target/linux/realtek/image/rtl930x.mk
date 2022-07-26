# SPDX-License-Identifier: GPL-2.0-only

define Device/zyxel_xgs1010-12
	SOC := rtl9302
	UIMAGE_MAGIC := 0x93001010
	ZYXEL_VERS := ABTY
	DEVICE_VENDOR := Zyxel
	DEVICE_MODEL := XGS1010-12
	IMAGE_SIZE := 13312k
	KERNEL_INITRAMFS := kernel-bin | append-dtb | gzip | zyxel-vers $$$$(ZYXEL_VERS) | \
	                    uImage gzip
endef
TARGET_DEVICES += zyxel_xgs1010-12

define Device/zyxel_xgs1210-12
  SOC := rtl9302
  UIMAGE_MAGIC := 0x93001210
  ZYXEL_VERS := ABTY
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := XGS1210-12
  IMAGE_SIZE := 13312k
  KERNEL_INITRAMFS := kernel-bin | append-dtb | gzip | zyxel-vers $$$$(ZYXEL_VERS) | \
        uImage gzip

endef
TARGET_DEVICES += zyxel_xgs1210-12

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

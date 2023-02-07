# SPDX-License-Identifier: GPL-2.0-only

define Device/zyxel_xgs1250-12
  SOC := rtl9302
  UIMAGE_MAGIC := 0x93001250
  ZYXEL_VERS := ABWE
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := XGS1250-12
  IMAGE_SIZE := 13312k
  KERNEL_INITRAMFS := \
	kernel-bin | \
	append-dtb | \
	gzip | \
	zyxel-vers $$$$(ZYXEL_VERS) | \
	uImage gzip
endef
TARGET_DEVICES += zyxel_xgs1250-12

define Device/hasivo_f1100w-8splus
  SOC := rtl9303
  DEVICE_VENDOR := hasivo
  DEVICE_MODEL := F1100W-8S+
    IMAGE_SIZE := 29696k
  KERNEL_INITRAMFS := kernel-bin | append-dtb | gzip | uImage gzip
endef

TARGET_DEVICES += hasivo_f1100w-8splus

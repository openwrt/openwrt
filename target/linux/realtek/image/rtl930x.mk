# SPDX-License-Identifier: GPL-2.0-only

define Device/zyxel_xgs1010-12
  SOC := rtl9302
  UIMAGE_MAGIC := 0x93001010
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := XGS1010-12
  IMAGE_SIZE := 15424k
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | uImage lzma
endef
TARGET_DEVICES += zyxel_xgs1010-12

define Device/zyxel_xgs1010-12_vendor
  SOC := rtl9302
  UIMAGE_MAGIC := 0x93001010
  ZYXEL_VERS := ABTY
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := XGS1010-12_vendor
  IMAGE_SIZE := 7232k
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | zyxel-vers $$$$(ZYXEL_VERS) | \
                      uImage lzma
endef
TARGET_DEVICES += zyxel_xgs1010-12_vendor

define Device/zyxel_xgs1210-12
  SOC := rtl9302
  UIMAGE_MAGIC := 0x93001210
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := XGS1210-12
  IMAGE_SIZE := 15424k
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | uImage lzma
endef
TARGET_DEVICES += zyxel_xgs1210-12

define Device/zyxel_xgs1210-12_vendor
  SOC := rtl9302
  UIMAGE_MAGIC := 0x93001210
  ZYXEL_VERS := ABTY
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := XGS1210-12_vendor
  IMAGE_SIZE := 13312k
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | zyxel-vers $$$$(ZYXEL_VERS) | \
                      uImage lzma
endef
TARGET_DEVICES += zyxel_xgs1210-12_vendor

define Device/zyxel_xgs1250-12
  SOC := rtl9302
  UIMAGE_MAGIC := 0x93001250
  ZYXEL_VERS := ABWE
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := XGS1250-12
  IMAGE_SIZE := 13312k
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | zyxel-vers $$$$(ZYXEL_VERS) | \
                      uImage lzma
endef
TARGET_DEVICES += zyxel_xgs1250-12

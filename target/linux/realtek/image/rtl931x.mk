# SPDX-License-Identifier: GPL-2.0-only
define Device/plasmacloud_psx28
  SOC := rtl9312
  UIMAGE_MAGIC := 0x93100000
  DEVICE_VENDOR := Plasma Cloud
  DEVICE_MODEL := PSX28/ESX28
  IMAGE_SIZE := 13376k
  BLOCKSIZE := 64k
  SUPPORTED_DEVICES = plasmacloud,psx28 plasmacloud,esx28
  DEVICE_PACKAGES := \
		     lm-sensors \
		     kmod-hwmon-adt7475 \
		     kmod-sched-act-police \
		     kmod-sched-flower \
		     poemgr \
		     i2c-tools
  KERNEL_INITRAMFS := \
    kernel-bin | \
    append-dtb | \
    lzma | \
    uImage lzma
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma | pad-to $$(BLOCKSIZE)
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | append-rootfs | pad-rootfs
  IMAGE/sysupgrade.bin := append-rootfs | pad-rootfs | sysupgrade-tar rootfs=$$$$@ | append-metadata
endef
TARGET_DEVICES += plasmacloud_psx28

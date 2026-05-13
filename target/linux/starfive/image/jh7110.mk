# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2025 Toco Technologies <info@toco.ae>
#
define Device/Default
  PROFILES := Default
  KERNEL_NAME := Image
  KERNEL := kernel-bin
  IMAGES := sdcard.img.gz
  IMAGE/sdcard.img.gz := boot-scr-jh7110 | riscv-sdcard | append-metadata | gzip
endef

define Device/visionfive2-v1.2a
  DEVICE_VENDOR := StarFive
  DEVICE_MODEL := VisionFive2 v1.2a
  DEVICE_DTS := starfive/jh7110-starfive-visionfive-2-v1.2a
  DEVICE_PACKAGES := kmod-eeprom-at24 kmod-leds-gpio
endef
TARGET_DEVICES += visionfive2-v1.2a

define Device/visionfive2-v1.3b
  DEVICE_VENDOR := StarFive
  DEVICE_MODEL := VisionFive2 v1.3b
  DEVICE_DTS := starfive/jh7110-starfive-visionfive-2-v1.3b
  DEVICE_PACKAGES := kmod-eeprom-at24 kmod-leds-gpio uboot-envtools
endef
TARGET_DEVICES += visionfive2-v1.3b

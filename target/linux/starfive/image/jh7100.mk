# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2025 Toco Technologies <info@toco.ae>
#

define Device/JH7100
  PROFILES := Default
  KERNEL_NAME := Image
  KERNEL := kernel-bin
  IMAGES := sdcard.img.gz
  IMAGE/sdcard.img.gz := boot-scr-jh7100 | riscv-sdcard | append-metadata | gzip
endef

define Device/beaglev-starlight
  $(call Device/JH7100)
  DEVICE_VENDOR := BeagleV
  DEVICE_MODEL := Starlight
  DEVICE_DTS := starfive/jh7100-beaglev-starlight
endef
TARGET_DEVICES += beaglev-starlight

define Device/visionfive-v1
  $(call Device/JH7100)
  DEVICE_VENDOR := StarFive
  DEVICE_MODEL := VisionFive v1
  DEVICE_DTS := starfive/jh7100-starfive-visionfive-v1
  DEVICE_PACKAGES := kmod-eeprom-at24 kmod-brcmfmac cypress-firmware-43430-sdio \
		     wpad-basic-mbedtls kmod-leds-gpio
endef
TARGET_DEVICES += visionfive-v1

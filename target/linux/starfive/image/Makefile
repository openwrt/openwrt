# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 Toco Technologies <info@toco.ae>
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

FAT32_BLOCK_SIZE=1024
FAT32_BLOCKS=$(shell echo $$(($(CONFIG_STARFIVE_SD_BOOT_PARTSIZE)*1024*1024/$(FAT32_BLOCK_SIZE))))

define Build/boot-scr-jh7110
	rm -f $@-boot.scr
	mkimage -A riscv -O linux -T script -C none -a 0 -e 0 -d mmc.bootscript.jh7110 $@-boot.scr
endef

define Build/boot-scr-jh7100
	rm -f $@-boot.scr
	mkimage -A riscv -O linux -T script -C none -a 0 -e 0 -d mmc.bootscript.jh7100 $@-boot.scr
endef

define Build/riscv-sdcard
	rm -f $@.boot #$(KDIR_TMP)/$(IMG_PREFIX)-$(PROFILE)-boot.img
	mkfs.fat $@.boot -C $(FAT32_BLOCKS)
	mcopy -i $@.boot $(DEVICE_DTS_DIR)/$(DEVICE_DTS).dtb ::dtb
	mcopy -i $@.boot $@-boot.scr ::boot.scr.uimg
	mcopy -i $@.boot $(IMAGE_KERNEL) ::Image
	mcopy -i $@.boot jh7110-vf2-uenv.txt ::vf2_uEnv.txt
	./gen_starfive_sdcard_img.sh \
		$@ \
		$@.boot \
		$(IMAGE_ROOTFS) \
		$(CONFIG_STARFIVE_SD_BOOT_PARTSIZE) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE)
endef

define Device/Default
  PROFILES := Default
  DEVICE_DTS_DIR := $(DTS_DIR)/starfive
  KERNEL_NAME := Image
  KERNEL_LOADADDR := 0x40200000
  KERNEL := kernel-bin
  KERNEL_INITRAMFS_SUFFIX := .itb
  KERNEL_INITRAMFS = kernel-bin | fit none $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  IMAGES := sdcard.img.gz
  IMAGE/sdcard.img.gz := boot-scr-jh7110 | riscv-sdcard | append-metadata | gzip
endef

define Device/JH7100
  PROFILES := Default
  DEVICE_DTS_DIR := $(DTS_DIR)/starfive
  KERNEL_NAME := Image
  KERNEL_LOADADDR := 0x84000000
  KERNEL := kernel-bin
  KERNEL_INITRAMFS_SUFFIX := .itb
  KERNEL_INITRAMFS = kernel-bin | fit none $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  IMAGES := sdcard.img.gz
  IMAGE/sdcard.img.gz := boot-scr-jh7100 | riscv-sdcard | append-metadata | gzip
endef

define Device/visionfive2-v1.2a
  DEVICE_VENDOR := StarFive
  DEVICE_MODEL := VisionFive2 v1.2a
  DEVICE_DTS := jh7110-starfive-visionfive-2-v1.2a
  DEVICE_PACKAGES := kmod-eeprom-at24 kmod-leds-gpio
endef
TARGET_DEVICES += visionfive2-v1.2a

define Device/visionfive2-v1.3b
  DEVICE_VENDOR := StarFive
  DEVICE_MODEL := VisionFive2 v1.3b
  DEVICE_DTS := jh7110-starfive-visionfive-2-v1.3b
  DEVICE_PACKAGES := kmod-eeprom-at24 kmod-leds-gpio uboot-envtools
endef
TARGET_DEVICES += visionfive2-v1.3b

define Device/beaglev-starlight
  $(call Device/JH7100)
  DEVICE_VENDOR := BeagleV
  DEVICE_MODEL := Starlight
  DEVICE_DTS := jh7100-beaglev-starlight
endef
TARGET_DEVICES += beaglev-starlight

define Device/visionfive-v1
  $(call Device/JH7100)
  DEVICE_VENDOR := StarFive
  DEVICE_MODEL := VisionFive v1
  DEVICE_DTS := jh7100-starfive-visionfive-v1
  DEVICE_PACKAGES := kmod-eeprom-at24 kmod-brcmfmac cypress-firmware-43430-sdio wpad-basic-mbedtls \
			kmod-leds-gpio
endef
TARGET_DEVICES += visionfive-v1

$(eval $(call BuildImage))

# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2012-2014 OpenWrt.org

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

FAT32_BLOCK_SIZE=1024
FAT32_BLOCKS=$(shell echo $$(($(CONFIG_OMAP_SD_BOOT_PARTSIZE)*1024*1024/$(FAT32_BLOCK_SIZE))))

UBIFS_OPTS = -F -m 2048 -e 124KiB -c 4096 -U
UBI_OPTS = -m 2048 -p 128KiB -s 512 -O 2048

define Build/omap-sdcard
	rm -f $@.boot
	mkfs.fat $@.boot -C $(FAT32_BLOCKS)

	mcopy -i $@.boot $(STAGING_DIR_IMAGE)/$(DEVICE_NAME)/MLO ::MLO
	mcopy -i $@.boot $(STAGING_DIR_IMAGE)/$(DEVICE_NAME)/u-boot.img ::u-boot.img
	mcopy -i $@.boot $(STAGING_DIR_IMAGE)/$(DEVICE_NAME)/boot.scr ::boot.scr
	mmd -i $@.boot ::/dtbs
	mcopy -i $@.boot $(DTS_DIR)/$(DEVICE_DTS).dtb ::/dtbs/$(DEVICE_DTS).dtb
	mcopy -i $@.boot $(IMAGE_KERNEL) ::/zImage
	./gen_omap_sdcard_img.sh $@ \
		$@.boot \
		$(IMAGE_ROOTFS) \
		$(CONFIG_OMAP_SD_BOOT_PARTSIZE) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE)
	rm -f $@.boot
endef

define Build/centurysys-sdcard
	rm -f $@.boot

	echo "#!/bin/sh" > $@.autoexec
	echo "global.linux.bootargs.base=\"\"" >> $@.autoexec
	echo "bootm /boot/uImage" >> $@.autoexec

	mkfs.fat $@.boot -C $(FAT32_BLOCKS)

	mcopy -i $@.boot $(IMAGE_KERNEL) ::/uImage
	mcopy -i $@.boot $@.autoexec ::/autoexec.sh
	./gen_omap_sdcard_img.sh $@ \
		$@.boot \
		$(IMAGE_ROOTFS) \
		$(CONFIG_OMAP_SD_BOOT_PARTSIZE) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE)
	rm -f $@.boot
endef

define Device/Default
  PROFILES := Default
  KERNEL_NAME := zImage
  KERNEL := kernel-bin
  DEVICE_DTS = $(lastword $(subst _, ,$(1)))
  IMAGES := sdcard.img.gz
  IMAGE/sdcard.img.gz := omap-sdcard | append-metadata | gzip
endef

#uboot-omap-am335x_evm uboot-omap-omap3_beagle uboot-omap-omap4_panda

define Device/centurysystems_ma-e350-n-mmc
  DEVICE_VENDOR := Century Systems
  DEVICE_MODEL := FutureNet MA-E350/N
  DEVICE_VARIANT := (MMC)
  DEVICE_DTS := am335x-ma-e350-n-mmc
  DEVICE_DTS_DIR := ../dts-$(KERNEL_PATCHVER)
  KERNEL_LOADADDR := 0x80008000
  KERNEL := kernel-bin | append-dtb | uImage none
  IMAGE/sdcard.img.gz := centurysys-sdcard | append-metadata | gzip
  DEVICE_PACKAGES := kmod-gpio-button-hotplug kmod-leds-tca6507 kmod-usb-acm
endef

TARGET_DEVICES += centurysystems_ma-e350-n-mmc

define Device/centurysystems_ma-e350-n-nand
  DEVICE_VENDOR := Century Systems
  DEVICE_MODEL := FutureNet MA-E350/N
  DEVICE_VARIANT := (NAND)
  DEVICE_DTS := am335x-ma-e350-n-nand
  DEVICE_DTS_DIR := ../dts-$(KERNEL_PATCHVER)
  KERNEL_LOADADDR := 0x80008000
  KERNEL := kernel-bin | append-dtb | uImage none
  KERNEL_IN_UBI := 1
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  UBINIZE_OPTS := -E 5
  FILESYSTEMS := squashfs
  IMAGES := sysupgrade.bin nand.ubi
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE/nand.ubi := append-ubi
  DEVICE_PACKAGES := kmod-gpio-button-hotplug kmod-leds-tca6507 kmod-usb-acm
endef

TARGET_DEVICES += centurysystems_ma-e350-n-nand

define Device/ti_am335x-evm
  DEVICE_VENDOR := Texas Instruments
  DEVICE_MODEL := AM335x EVM
endef

TARGET_DEVICES += ti_am335x-evm

define Device/ti_am335x-bone-black
  DEVICE_VENDOR := Texas Instruments
  DEVICE_MODEL := AM335x BeagleBone Black
  DEVICE_DTS := am335x-boneblack
endef

TARGET_DEVICES += ti_am335x-bone-black

define Device/ti_omap4-panda
  DEVICE_VENDOR := PandaBoard.org
  DEVICE_MODEL := OMAP4 TI pandaboard
  DEVICE_PACKAGES := kmod-usb-net-smsc95xx
endef

TARGET_DEVICES += ti_omap4-panda

define Device/ti_omap3-beagle
  DEVICE_VENDOR := BeagleBoard.org
  DEVICE_MODEL := OMAP3 TI beagleboard
  # beagleboard doesn't have a network interface, support most common usb net
  DEVICE_PACKAGES := \
	kmod-usb-net-asix kmod-usb-net-asix-ax88179 kmod-usb-net-hso \
	kmod-usb-net-kaweth kmod-usb-net-pegasus kmod-usb-net-mcs7830 \
	kmod-usb-net-smsc95xx kmod-usb-net-dm9601-ether
endef

TARGET_DEVICES += ti_omap3-beagle

$(eval $(call BuildImage))

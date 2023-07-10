# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2013-2015 OpenWrt.org

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

FAT32_BLOCK_SIZE=1024
FAT32_BLOCKS=$(shell echo $$(($(CONFIG_MXS_SD_BOOT_PARTSIZE)*1024*1024/$(FAT32_BLOCK_SIZE))))

KERNEL_LOADADDR:=0x40008000

define Build/mxs-sdcard-ext4-ext4
	./gen_sdcard_ext4_ext4.sh \
		$@ \
		$(STAGING_DIR_IMAGE)/$(DEVICE_NAME)/u-boot.sb \
		$(IMAGE_ROOTFS) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE)
endef

define Build/mxs-sdcard-vfat-ext4
	rm -f $@.boot
	mkfs.fat $@.boot -C $(FAT32_BLOCKS)

	mcopy -i $@.boot $(DTS_DIR)/$(DEVICE_DTS).dtb ::$(DEVICE_DTS).dtb
	mcopy -i $@.boot $(IMAGE_KERNEL) ::uImage

	./gen_sdcard_vfat_ext4.sh \
		$@ \
		$(STAGING_DIR_IMAGE)/$(DEVICE_NAME)/u-boot.sb \
		$@.boot \
		$(IMAGE_ROOTFS) \
		$(CONFIG_MXS_SD_BOOT_PARTSIZE) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE)
endef

define Device/Default
  PROFILES := Default
  KERNEL_NAME := zImage
  KERNEL := kernel-bin | uImage none
  IMAGES := sdcard.img.gz
  DEVICE_DTS = $$(SOC)-$(lastword $(subst _, ,$(1)))
endef

define Device/i2se_duckbill
  DEVICE_VENDOR := I2SE
  DEVICE_MODEL := Duckbill
  DEVICE_PACKAGES := -dnsmasq -firewall -ppp -ip6tables -iptables -6relayd -mtd \
		     uboot-envtools kmod-leds-gpio -kmod-ipt-nathelper
  SUPPORTED_DEVICES:=i2se,duckbill
  SOC:=imx28
  DEVICE_DTS:=imx28-duckbill
  IMAGE/sdcard.img.gz = mxs-sdcard-ext4-ext4 | append-metadata | gzip
endef
TARGET_DEVICES += i2se_duckbill

define Device/olinuxino_maxi
  DEVICE_VENDOR := Olimex
  DEVICE_MODEL := OLinuXino Maxi
  DEVICE_PACKAGES := kmod-usb-net-smsc95xx kmod-pinctrl-mcp23s08-i2c \
		     kmod-pinctrl-mcp23s08-spi kmod-leds-gpio kmod-sound-core
  SUPPORTED_DEVICES:=olimex,imx23-olinuxino
  SOC:=imx23
  DEVICE_DTS:=imx23-olinuxino
  IMAGE/sdcard.img.gz = mxs-sdcard-vfat-ext4 | append-metadata | gzip
endef
TARGET_DEVICES += olinuxino_maxi

define Device/olinuxino_micro
  DEVICE_VENDOR := Olimex
  DEVICE_MODEL := OLinuXino Micro
  DEVICE_PACKAGES := kmod-pinctrl-mcp23s08-spi kmod-pinctrl-mcp23s08-i2c \
		     kmod-leds-gpio
  SUPPORTED_DEVICES:=olimex,imx23-olinuxino
  SOC:=imx23
  DEVICE_DTS:=imx23-olinuxino
  IMAGE/sdcard.img.gz = mxs-sdcard-vfat-ext4 | append-metadata | gzip
endef
TARGET_DEVICES += olinuxino_micro

$(eval $(call BuildImage))

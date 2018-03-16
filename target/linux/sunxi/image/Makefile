#
# Copyright (C) 2013-2016 OpenWrt.org
# Copyright (C) 2016 Yousong Zhou
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

FAT32_BLOCK_SIZE=1024
FAT32_BLOCKS=$(shell echo $$(($(CONFIG_SUNXI_SD_BOOT_PARTSIZE)*1024*1024/$(FAT32_BLOCK_SIZE))))

KERNEL_LOADADDR:=0x40008000

define Build/sunxi-sdcard-boot-fat
	rm -f $@.boot
	mkfs.fat $@.boot -C $(FAT32_BLOCKS)
	mkdir -p $(STAGING_DIR_IMAGE)/tmp/boot
	$(CP) $(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-boot.scr $(STAGING_DIR_IMAGE)/tmp/boot/boot.scr
	$(CP) $(DTS_DIR)/$(SUNXI_DTS).dtb $(STAGING_DIR_IMAGE)/tmp/boot/dtb
	$(CP)  $(IMAGE_KERNEL) $(STAGING_DIR_IMAGE)/tmp/boot/uImage
	 mcopy -i $@.boot -s $(STAGING_DIR_IMAGE)/tmp/boot ::/
	$(call Build/sunxi-sdcard-rootfs,SUNXI_BOOT_FAT)
	rm -f $@.boot
	rm -fr  $(STAGING_DIR_IMAGE)/tmp/boot
endef

define Build/sunxi-sdcard-boot-ext4
	$(call Build/sunxi-sdcard-rootfs,SUNXI_BOOT_EXT4)
endef

define Build/sunxi-sdcard-rootfs
	./gen_sunxi_sdcard_img.sh $@ \
		$@.boot \
		$(IMAGE_ROOTFS) \
		$(CONFIG_SUNXI_SD_BOOT_PARTSIZE) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) \
		$(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-u-boot-with-spl.bin \
		$(1)
endef

define Build/sunxi-sdcard
 $(if $(findstring -ext4-,$@),$(if $(CONFIG_TARGET_SUNXI_BOOTFS_EXT4_ONLY),\
  $(call Build/sunxi-sdcard-boot-ext4),$(call Build/sunxi-sdcard-boot-fat)),$(call Build/sunxi-sdcard-boot-fat))
endef

# why \x00\x00\x00\x00 for zImage-initramfs
define Device/Default
  PROFILES := Default
  DEVICE_VARS := SUNXI_DTS SUNXI_UBOOT
  KERNEL_NAME := zImage
  KERNEL := kernel-bin | uImage none
  IMAGES := sdcard.img.gz
  IMAGE/sdcard.img.gz := sunxi-sdcard | append-metadata | gzip
  SUNXI_BOOTFS := SUNXI_BOOT_FAT
endef

include cortex-a7.mk
include cortex-a8.mk
include cortex-a53.mk

$(eval $(call BuildImage))

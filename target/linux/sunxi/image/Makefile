# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2013-2016 OpenWrt.org
# Copyright (C) 2016 Yousong Zhou

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

FAT32_BLOCK_SIZE=1024
FAT32_BLOCKS=$(shell echo $$(($(CONFIG_SUNXI_SD_BOOT_PARTSIZE)*1024*1024/$(FAT32_BLOCK_SIZE))))

DEVICE_VARS := SUNXI_DTS SUNXI_DTS_DIR

define Build/boot-scr-emmc
	rm -f $@-boot.scr
	mkimage -A arm -O linux -T script -C none -a 0 -e 0 -d mmc.bootscript.ttyS5-emmc.txt $(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-emmcboot.scr
endef

define Build/sunxi-sdcard
	rm -f $@.boot
	mkfs.fat $@.boot -C $(FAT32_BLOCKS)

	mcopy -i $@.boot $(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-boot.scr ::boot.scr
	mcopy -i $@.boot $(DTS_DIR)/$(SUNXI_DTS).dtb ::dtb
	mcopy -i $@.boot $(IMAGE_KERNEL) ::uImage
	./gen_sunxi_sdcard_img.sh $@ \
		$@.boot \
		$(IMAGE_ROOTFS) \
		$(CONFIG_SUNXI_SD_BOOT_PARTSIZE) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) \
		$(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-u-boot-with-spl.bin
	rm -f $@.boot
endef

define Build/sunxi-emmc
	rm -f $@.boot-emmc
	mkfs.fat $@.boot-emmc -C $(FAT32_BLOCKS)

	mcopy -i $@.boot-emmc $(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-emmcboot.scr ::boot.scr
	mcopy -i $@.boot-emmc $(DTS_DIR)/$(SUNXI_DTS).dtb ::dtb
	mcopy -i $@.boot-emmc $(IMAGE_KERNEL) ::uImage

	./gen_sunxi_emmc_img.sh $@ \
		$@.boot-emmc \
		$(IMAGE_ROOTFS) \
		$(CONFIG_SUNXI_SD_BOOT_PARTSIZE) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE)
	rm -f $@.boot
endef

define Build/sunxi-emmc-uboot-copy
	$(CP) $(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-u-boot-with-spl.bin $(BIN_DIR)/
endef

define Device/Default
  PROFILES := Default
  KERNEL_NAME := zImage
  KERNEL := kernel-bin | uImage none
  IMAGES := sdcard.img.gz
  IMAGE/sdcard.img.gz := sunxi-sdcard | append-metadata | gzip
  SUNXI_DTS_DIR :=allwinner/
  SUNXI_DTS = $$(SUNXI_DTS_DIR)$$(SOC)-$(lastword $(subst _, ,$(1)))
endef

include $(SUBTARGET).mk

$(eval $(call BuildImage))

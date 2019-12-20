#
# Copyright (C) 2012-2015 OpenWrt.org
# Copyright (C) 2016-2017 LEDE project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

FAT32_BLOCK_SIZE=1024
FAT32_BLOCKS=$(shell echo $$(($(CONFIG_MEDIATEK_SD_BOOT_PARTSIZE)*1024*1024/$(FAT32_BLOCK_SIZE))))

# for arm
KERNEL_LOADADDR := 0x80008000

define Build/mediatek-sdcard
        rm -f $@.boot
        mkfs.fat $@.boot -C $(FAT32_BLOCKS)

        mcopy -i $@.boot $(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-uEnv.txt ::uEnv.txt
        mcopy -i $@.boot $(IMAGE_KERNEL) ::uImage
        ./gen_mediatek_sdcard_img.sh $@ \
                $(STAGING_DIR_IMAGE)/bpi-r2-preloader.bin \
                $(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-uboot-mediatek.bin \
                $@.boot \
                $(IMAGE_ROOTFS) \
                $(CONFIG_MEDIATEK_SD_BOOT_PARTSIZE) \
                $(CONFIG_TARGET_ROOTFS_PARTSIZE)
        rm -f $@.boot

endef

# for arm64
ifeq ($(SUBTARGET),mt7622)
KERNEL_LOADADDR = 0x41080000
endif

ifeq ($(SUBTARGET),mt7629)
KERNEL_LOADADDR = 0x40008000
endif

define Build/sysupgrade-emmc
	rm -f $@.recovery
	mkfs.fat -C $@.recovery 3070

	./gen_mt7623_emmc_img.sh $@ \
		$(IMAGE_KERNEL) \
		$@.recovery \
		$(IMAGE_ROOTFS)
endef

# default all platform image(fit) build 
define Device/Default
  PROFILES = Default $$(DEVICE_NAME)
  KERNEL_NAME := zImage
  FILESYSTEMS := squashfs
  DEVICE_DTS_DIR := $(DTS_DIR)
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata
  SUPPORTED_DEVICES := $(subst _,$(comma),$(1))
ifeq ($(SUBTARGET),mt7623)
  DEVICE_VARS := MEDIATEK_UBOOT
  KERNEL_NAME := zImage
  KERNEL := kernel-bin | append-dtb | uImage none
  KERNEL_INITRAMFS := kernel-bin | append-dtb | uImage none
  FILESYSTEMS += ext4
  IMAGES := sysupgrade.tar sdcard.img.gz
  IMAGE/sysupgrade.tar := sysupgrade-tar | append-metadata
  IMAGE/sdcard.img.gz := mediatek-sdcard | gzip | append-metadata
endif
ifeq ($(SUBTARGET),mt7622)
  KERNEL_NAME := Image
  KERNEL = kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS = kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
endif
endef

ifeq ($(SUBTARGET),mt7622)
include mt7622.mk
endif

ifeq ($(SUBTARGET),mt7623)
include mt7623.mk
endif

ifeq ($(SUBTARGET),mt7629)
include mt7629.mk
endif

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))


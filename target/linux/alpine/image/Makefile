# Copyright (c) 2026 The Linux Foundation. All rights reserved.
# Abdulkader Alrezej <alrazj.abdulkader@gmail.com>
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

DEVICE_VARS += NETGEAR_BOARD_ID NETGEAR_HW_ID

define Device/Default
	PROFILES := Default
	KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
	KERNEL_LOADADDR = 0x8000
	IMAGES := sysupgrade.bin
	IMAGE/sysupgrade.bin = sysupgrade-tar | append-metadata
	IMAGE/sysupgrade.bin/squashfs :=
endef

define Device/FitImage
	KERNEL_SUFFIX := -uImage.itb
	KERNEL = kernel-bin | gzip | fit gzip $$(KDIR)/image-$$(DEVICE_DTS).dtb
	KERNEL_NAME := Image
endef

define Build/mikrotik-elf
	$(TARGET_CROSS)objcopy -I binary -O elf32-littlearm \
		--rename-section .data=.text,alloc,code,load,readonly,contents \
		$@ $@.o
	$(TARGET_CROSS)ld -Ttext 0x00008000 -e 0x00008000 -o $@.elf $@.o
	mv $@.elf $@
endef

define Device/MikroTikImage
	KERNEL_NAME := zImage
	KERNEL_INITRAMFS_NAME := zImage-initramfs
	KERNEL := kernel-bin | append-dtb | mikrotik-elf
	KERNEL_INITRAMFS := kernel-bin | append-dtb | mikrotik-elf
	UBINIZE_OPTS := -E 5
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	IMAGES := sysupgrade.bin
	IMAGE/sysupgrade.bin := append-kernel | sysupgrade-tar | append-metadata
endef

define Device/mikrotik_rb1100ahx4
	$(call Device/MikroTikImage)
	DEVICE_VENDOR := MikroTik
	DEVICE_MODEL := RouterBOARD 1100AHx4
	DEVICE_DTS := alpine-rb1100ahx4
	DEVICE_NAME := mikrotik_rb1100ahx4
	SOC := al21400
	BLOCKSIZE := 131072
	PAGESIZE := 2048
	NAND_SIZE := 128m
	DEVICE_PACKAGES := kmod-dsa-realtek kmod-dsa-rtl8365mb \
						kmod-libphy kmod-mii kmod-fixed-phy kmod-phylink \
						-kmod-swconfig -swconfig
	KERNEL := kernel-bin | append-dtb | mikrotik-elf
	IMAGE/sysupgrade.bin := append-kernel | sysupgrade-tar | append-metadata
endef

TARGET_DEVICES += mikrotik_rb1100ahx4

$(eval $(call BuildImage))

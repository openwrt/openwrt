# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2006-2021 OpenWrt.org

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

# Cook a Linksys NSLU2 etc image
define Build/linksys-ixp425-image
	touch $@.null-initrd
	$(TOPDIR)/scripts/slugimage.pl -L $(STAGING_DIR_IMAGE)/apex/apex-$(1)-armeb.bin -k $@ -r $@.null-initrd -p -o $@.new
	mv $@.new $@
endef

# Build sysupgrade image
define BuildFirmware/Generic
	dd if=$(KDIR)/zImage of=$(KDIR)/zImage.pad bs=64k conv=sync; \
	dd if=$(KDIR)/root.$(1) of=$(KDIR)/root.$(1).pad bs=128k conv=sync; \
	sh $(TOPDIR)/scripts/combined-image.sh \
		$(KDIR)/zImage.pad \
		$(KDIR)/root.$(1).pad \
		$(BIN_DIR)/$(IMG_PREFIX)-$(patsubst jffs2-%,jffs2,$(patsubst squashfs-%,squashfs,$(1)))-sysupgrade.bin
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	$(call BuildFirmware/Generic,$(1))
endef

define Device/Default
	PROFILES := Default
	KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
	KERNEL_NAME := zImage
	KERNEL := kernel-bin | append-dtb
	BLOCKSIZE := 128k
endef

define Device/gateworks_avila
	DEVICE_VENDOR := Gateworks
	DEVICE_MODEL := Avila GW2348-4
	DEVICE_PACKAGES := ixp4xx-microcode-ethernet kmod-rtc-ds1672 kmod-eeprom-at24 kmod-hwmon-ad7418
	DEVICE_DTS := intel-ixp42x-gateworks-gw2348
	KERNEL := kernel-bin | append-dtb
	IMAGES := kernel.bin rootfs.bin
	IMAGE/kernel.bin := append-kernel
	IMAGE/rootfs.bin := append-rootfs | pad-rootfs | pad-to 128k
endef
TARGET_DEVICES += gateworks_avila

define Device/gateworks_cambria
	DEVICE_VENDOR := Gateworks
	DEVICE_MODEL := Cambria GW2358-4
	DEVICE_PACKAGES := ixp4xx-microcode-ethernet kmod-rtc-ds1672 kmod-eeprom-at24 kmod-hwmon-ad7418
	DEVICE_DTS := intel-ixp43x-gateworks-gw2358
	KERNEL := kernel-bin | append-dtb
	IMAGES := kernel.bin rootfs.bin
	IMAGE/kernel.bin := append-kernel
	IMAGE/rootfs.bin := append-rootfs | pad-rootfs | pad-to 128k
endef
TARGET_DEVICES += gateworks_cambria

define Device/linksys_nslu2
	DEVICE_VENDOR := Linksys
	DEVICE_MODEL := NSLU2
	# USB2 is compiled in and needs no package
	DEVICE_PACKAGES := ixp4xx-microcode-ethernet kmod-rtc-x1205
	# Only 32 MB of RAM so not building by default
	DEFAULT := n
	DEVICE_DTS := intel-ixp42x-linksys-nslu2
	KERNEL := kernel-bin | append-dtb
	IMAGES := factory.bin
	# This has to boot from harddisk so just append the kernel
	IMAGE/factory.bin := append-kernel | linksys-ixp425-image "nslu2"
endef
TARGET_DEVICES += linksys_nslu2

define Device/usrobotics_usr8200
	DEVICE_VENDOR := USRobotics
	DEVICE_MODEL := USR8200
	# USB2 is compiled in and needs no package
	DEVICE_PACKAGES := ixp4xx-microcode-ethernet kmod-rtc-r7301 kmod-firewire kmod-firewire-ohci
	DEVICE_DTS := intel-ixp42x-usrobotics-usr8200
	KERNEL := kernel-bin | append-dtb
	IMAGES := kernel.bin rootfs.bin
	IMAGE/kernel.bin := append-kernel
	IMAGE/rootfs.bin := append-rootfs | pad-rootfs | pad-to 128k
endef
TARGET_DEVICES += usrobotics_usr8200

$(eval $(call BuildImage))

#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Prepare
# WBD111: mach id 1690 (0x69a)
	echo -en "\x06\x1c\xa0\xe3\x9a\x10\x81\xe3" > $(KDIR)/openwrt-$(BOARD)-wbd111-zImage
	cat $(LINUX_DIR)/arch/arm/boot/zImage >> $(KDIR)/openwrt-$(BOARD)-wbd111-zImage
endef

define Image/BuildKernel
	cp $(KDIR)/openwrt-$(BOARD)-wbd111-zImage $(BIN_DIR)/openwrt-$(BOARD)-wbd111-zImage
endef

define Image/Build/jffs2-64k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=64k conv=sync
endef

define Image/Build/jffs2-128k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=128k conv=sync
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1),$(1))

	-$(STAGING_DIR_HOST)/bin/mkfwimage2 \
		-m GEOS -f 0x30000000 -z \
		-v WILI-S.WILIBOARD.v5.00.SL3512.OpenWrt.00000.000000.000000 \
		-o $(BIN_DIR)/openwrt-$(BOARD)-wbd111-$(1).bin \
		-p Kernel:0x020000:0x100000:0:0:$(BIN_DIR)/openwrt-$(BOARD)-wbd111-zImage \
		-p Ramdisk:0x120000:0x500000:0:0:$(BIN_DIR)/openwrt-$(BOARD)-$(1).img
endef

$(eval $(call BuildImage))

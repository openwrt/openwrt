#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Prepare
	cp $(LINUX_DIR)/arch/arm/boot/zImage $(KDIR)/zImage
endef

define Image/BuildKernel
	cp $(KDIR)/zImage $(BIN_DIR)/openwrt-$(BOARD)-zImage
#
# XXX - FIXME
#
#	BIN_DIR=$(BIN_DIR) $(TOPDIR)/scripts/arm-magic.sh
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
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-root.$(1) bs=128k conv=sync

	-$(STAGING_DIR_HOST)/bin/mkfwimage2 \
		-m GEOS -f 0x30000000 -z \
		-v WILI-S.WILIBOARD.v5.00.SL3512.OpenWrt.00000.000000.000000 \
		-o $(BIN_DIR)/openwrt-$(BOARD)-wbd-111-$(1).bin \
		-p Kernel:0x020000:0x100000:0:0:$(BIN_DIR)/openwrt-$(BOARD)-zImage \
		-p Ramdisk:0x120000:0x500000:0:0:$(BIN_DIR)/openwrt-$(BOARD)-root.$(1)
endef

$(eval $(call BuildImage))

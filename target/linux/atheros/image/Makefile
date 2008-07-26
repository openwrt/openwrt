# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/openwrt-$(BOARD)-vmlinux.elf
	gzip -9 -c $(KDIR)/vmlinux > $(KDIR)/vmlinux.bin.gz
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/vmlinux $(KDIR)/vmlinux.bin.l7
	dd if=$(KDIR)/vmlinux.bin.l7 of=$(BIN_DIR)/openwrt-$(BOARD)-vmlinux.lzma bs=65536 conv=sync
	dd if=$(KDIR)/vmlinux.bin.gz of=$(BIN_DIR)/openwrt-$(BOARD)-vmlinux.gz bs=65536 conv=sync
endef

define Image/Build/squashfs
    $(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-root.$(1) bs=128k conv=sync

	-$(STAGING_DIR_HOST)/bin/mkfwimage -k $(BIN_DIR)/openwrt-$(BOARD)-vmlinux.lzma \
		-r $(BIN_DIR)/openwrt-$(BOARD)-root.$(1) \
		-v XS2.ar2316.OpenWRT \
		-o $(BIN_DIR)/openwrt-$(BOARD)-ubnt2-$(1).bin
	-$(STAGING_DIR_HOST)/bin/mkfwimage -k $(BIN_DIR)/openwrt-$(BOARD)-vmlinux.lzma \
		-r $(BIN_DIR)/openwrt-$(BOARD)-root.$(1) \
		-v XS5.ar2313.OpenWRT \
		-o $(BIN_DIR)/openwrt-$(BOARD)-ubnt5-$(1).bin \
		-s 0xBE030000
endef

$(eval $(call BuildImage))

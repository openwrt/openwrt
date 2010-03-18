# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/BuildKernel
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/vmlinux $(KDIR)/vmlinux.lzma
	mkimage -A mips -O linux -T kernel -a 0x80002000 -C lzma -e \
		0x80002000 \
		-n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/vmlinux.lzma $(KDIR)/uImage

	cp $(KDIR)/uImage $(BIN_DIR)/openwrt-$(BOARD)-uImage
endef

define Image/Build/squashfs
	cat $(KDIR)/uImage $(KDIR)/root.$(1) > $(BIN_DIR)/openwrt-$(BOARD)-$(1).image
	$(call prepare_generic_squashfs,$(BIN_DIR)/openwrt-$(BOARD)-$(1).image)
endef

define Image/Build/jffs2-64k
	dd if=$(KDIR)/uImage of=$(KDIR)/uImage.$(1) bs=64k conv=sync
	cat $(KDIR)/uImage.$(1) $(KDIR)/root.$(1) > $(BIN_DIR)/openwrt-$(BOARD)-$(1).image
endef

define Image/Build/jffs2-128k
	dd if=$(KDIR)/uImage of=$(KDIR)/uImage.$(1) bs=128k conv=sync
	cat $(KDIR)/uImage.$(1) $(KDIR)/root.$(1) > $(BIN_DIR)/openwrt-$(BOARD)-$(1).image
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))

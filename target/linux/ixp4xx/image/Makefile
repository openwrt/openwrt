# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Build/slug
	BIN_DIR=$(BIN_DIR) $(TOPDIR)/scripts/slugimage.pl \
		-L $(KDIR)/apex-nslu2-armeb.bin \
		-k $(BIN_DIR)/openwrt-nslu2-zImage \
		-r rootfs:$(BIN_DIR)/openwrt-$(BOARD)-$(1).img \
		-p -o $(BIN_DIR)/openwrt-nslu2-$(1).bin
	BIN_DIR=$(BIN_DIR) $(TOPDIR)/scripts/slugimage.pl \
		-F -L $(KDIR)/apex-nslu2-16mb-armeb.bin \
		-k $(BIN_DIR)/openwrt-nslu2-zImage \
		-r rootfs:$(BIN_DIR)/openwrt-$(BOARD)-$(1).img \
		-p -o $(BIN_DIR)/openwrt-nslu2-$(1)-16mb.bin
endef

define Build/Compile
	$(MAKE) -C apex \
		BUILD_DIR="$(KDIR)" \
		TARGET="$(KDIR)" \
		compile	
endef

define Build/Clean
	$(MAKE) -C apex clean
endef

define Image/Prepare
	cp $(LINUX_DIR)/arch/arm/boot/zImage $(KDIR)/zImage
endef

define Image/BuildKernel
	cp $(KDIR)/zImage $(BIN_DIR)/openwrt-$(BOARD)-zImage
	BIN_DIR=$(BIN_DIR) $(TOPDIR)/scripts/arm-magic.sh
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

define Image/Build/jffs2-64k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=65536 conv=sync
endef

define Image/Build/jffs2-128k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=131072 conv=sync
	$(call Image/Build/slug,$(1))
endef

define Image/Build/squashfs
    $(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=131072 conv=sync
	$(call Image/Build/slug,$(1))
endef

$(eval $(call BuildImage))

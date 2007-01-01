# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/Compile
	$(MAKE) -C apex \
		BUILD_DIR="$(KDIR)" \
		TARGET="$(KDIR)" \
		install
	$(MAKE) -C npe-ucode \
		BUILD_DIR="$(KDIR)" \
		TARGET="$(KDIR)" \
		install
endef

define Build/Clean
	$(MAKE) -C apex clean
	$(MAKE) -C npe-ucode clean
endef

define Image/Prepare
endef

define Image/BuildKernel
	cp $(LINUX_DIR)/arch/arm/boot/zImage $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-zImage
#	$(shell BIN_DIR=$(BIN_DIR) $(TOPDIR)/scripts/arm-magic.sh)
	BIN_DIR=$(BIN_DIR) $(TOPDIR)/scripts/arm-magic.sh
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

define Image/Build/jffs2-64k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img bs=65536 conv=sync
endef

define Image/Build/jffs2-128k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img bs=131072 conv=sync
	BIN_DIR=$(BIN_DIR) $(TOPDIR)/scripts/slugimage.pl \
		-L $(KDIR)/apex-nslu2-armeb.bin -m $(KDIR)/NPE-B \
		-k $(BIN_DIR)/openwrt-nslu2-$(KERNEL)-zImage \
		-r Flashdisk:$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img \
		-p -o $(BIN_DIR)/openwrt-nslu2-$(KERNEL)-$(1).bin
endef

$(eval $(call BuildImage))

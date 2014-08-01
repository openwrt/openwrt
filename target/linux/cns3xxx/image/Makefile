#
# Copyright (C) 2010-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Prepare
	mkimage -A arm -O linux -T kernel -C none -a 0x20008000 -e 0x20008000 -n 'OpenWrt Linux-$(LINUX_VERSION)' -d $(KDIR)/zImage $(KDIR)/uImage
endef

# Build sysupgrade image
define BuildFirmware/Generic
	dd if=$(KDIR)/uImage of=$(KDIR)/uImage.pad bs=64k conv=sync; \
	dd if=$(KDIR)/root.$(1) of=$(KDIR)/root.$(1).pad bs=128k conv=sync; \
	sh $(TOPDIR)/scripts/combined-image.sh \
		$(KDIR)/uImage.pad \
		$(KDIR)/root.$(1).pad \
		$(BIN_DIR)/$(IMG_PREFIX)-$(patsubst jffs2-%,jffs2,$(patsubst squashfs-%,squashfs,$(1)))-uboot-sysupgrade.bin
	sh $(TOPDIR)/scripts/combined-image.sh \
		$(KDIR)/uImage.pad \
		$(KDIR)/root.$(1).pad \
		$(BIN_DIR)/$(IMG_PREFIX)-$(patsubst jffs2-%,jffs2,$(patsubst squashfs-%,squashfs,$(1)))-sysupgrade.bin
endef

define Image/BuildKernel/Initramfs
	mkimage -A arm -O linux -T kernel -C none -a 0x20008000 -e 0x20008000 -n 'OpenWrt Linux-$(LINUX_VERSION)' -d $(KDIR)/zImage-initramfs $(BIN_DIR)/$(IMG_PREFIX)-initramfs-uImage
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	$(call BuildFirmware/Generic,$(1))
	cp $(KDIR)/uImage $(BIN_DIR)/$(IMG_PREFIX)-uImage
endef

define Image/Build/jffs2-64k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=64k conv=sync
	( \
	dd if=$(KDIR)/uImage bs=2048k conv=sync; \
	dd if=$(KDIR)/root.$(1) bs=64k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(1).bin
endef

define Image/Build/jffs2-128k
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=128k conv=sync
	( \
	dd if=$(KDIR)/uImage bs=2048k conv=sync; \
	dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(1).bin
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(1).img bs=128k conv=sync
	( \
	dd if=$(KDIR)/uImage bs=2048k conv=sync; \
	dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(1)_laguna_nor.bin
	( \
	dd if=$(KDIR)/uImage bs=1536k conv=sync; \
	dd if=$(KDIR)/root.$(1) bs=256k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(1)_laguna_spi.bin
endef

$(eval $(call BuildImage))

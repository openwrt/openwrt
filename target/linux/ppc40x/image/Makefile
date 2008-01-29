#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2_BLOCKSIZE=128k

define Image/Prepare
	cp $(LINUX_DIR)/arch/powerpc/boot/uImage $(KDIR)/uImage
	dtc -O dtb -R 4 -S 0x20000 $(LINUX_DIR)/arch/powerpc/boot/dts/kilauea.dts > $(KDIR)/openwrt-kilauea.dtb
endef

define Image/BuildKernel
	cp $(KDIR)/uImage $(BIN_DIR)/openwrt-$(BOARD)-uImage
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

define Image/Build/jffs2-128k
	( \
		dd if=$(LINUX_DIR)/arch/powerpc/boot/uImage bs=1920k conv=sync; \
		dd if=$(KDIR)/openwrt-kilauea.dtb bs=128k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/openwrt-$(BOARD)-jffs2.img
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	( \
		dd if=$(LINUX_DIR)/arch/powerpc/boot/uImage bs=1920k conv=sync; \
		dd if=$(KDIR)/openwrt-kilauea.dtb bs=128k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/openwrt-$(BOARD)-$(1).img
endef

$(eval $(call BuildImage))

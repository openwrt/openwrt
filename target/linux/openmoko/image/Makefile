# 
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

JFFS2_BLOCKSIZE := 128k

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/mkfs/jffs2/sub
	$(STAGING_DIR_HOST)/bin/mkfs.jffs2 --pad --little-endian --pagesize=0x800 --no-cleanmarkers -e $(patsubst %k,%KiB,$(1)) -o $(KDIR)/root.jffs2-$(1) -d $(TARGET_DIR)
	$(call add_jffs2_mark,$(KDIR)/root.jffs2-$(1))
	$(call Image/Build,jffs2-$(1))
endef

define Build/Clean
endef

define Build/Compile
endef

define Image/Prepare
	cp $(LINUX_DIR)/arch/arm/boot/uImage $(KDIR)/uImage
endef

define Image/BuildKernel
	cp $(KDIR)/uImage $(BIN_DIR)/openwrt-$(BOARD)-uImage
endef

define Image/Build
	cp $(KDIR)/root.jffs2-128k $(BIN_DIR)/openwrt-$(BOARD)-rootfs
endef

$(eval $(call BuildImage))

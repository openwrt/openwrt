# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/Clean
endef

define Image/Prepare
endef

define Image/Build
	touch $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img
	$(STAGING_DIR)/bin/airlink -b 0 -j $(LINUX_DIR)/arch/i386/boot/bzImage $(KDIR)/root.$(1) $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).img
endef

$(eval $(call BuildImage))

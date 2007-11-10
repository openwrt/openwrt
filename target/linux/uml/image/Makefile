# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Prepare
	cp $(LINUX_DIR)/linux $(KDIR)/vmlinux.elf
endef

define Image/Build
	cp $(KDIR)/root.$(1) $(BIN_DIR)/openwrt-$(BOARD)-$(1).img
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/openwrt-$(BOARD)-vmlinux
endef

$(eval $(call BuildImage))

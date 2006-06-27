# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

LINUX_VERSION:=2.6.8.1
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=9517ca999e822b898fbdc7e72796b1aa

include ./config
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/kernel-build.mk


$(LINUX_DIR)/.patched: $(LINUX_DIR)/.unpacked
	#[ -d ../generic-$(KERNEL)/patches ] && $(PATCH) $(LINUX_DIR) ../generic-$(KERNEL)/patches $(MAKE_TRACE)
	[ -d ./patches ] && $(PATCH) $(LINUX_DIR) ./patches $(MAKE_TRACE)
	@$(CP) config $(LINUX_DIR)/.config
	@$(CP) ./files/built-in.o $(LINUX_DIR)/bcmdrivers/broadcom/
	touch $@


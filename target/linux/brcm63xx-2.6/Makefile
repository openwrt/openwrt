# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=brcm63xx
BOARDNAME:=Broadcom BCM963xx
FEATURES:=squashfs jffs2 broken usb atm

define Target/Description
	Build firmware images for Broadcom based xDSL/routers
	(e.g. Inventel Livebox, Siemens SE515)
endef

include $(INCLUDE_DIR)/kernel-build.mk

# include the profiles
-include profiles/*.mk

$(eval $(call BuildKernel))

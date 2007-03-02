# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=brcm
BOARDNAME:=Broadcom BCM947xx/953xx
FEATURES:=squashfs

define Target/Description
	Build firmware images for Broadcom based routers
	(e.g. Netgear WGT634U)
endef

include $(INCLUDE_DIR)/kernel-build.mk
DEFAULT_PACKAGES += kmod-switch kmod-diag

# include the profiles
-include profiles/*.mk

$(eval $(call BuildKernel))

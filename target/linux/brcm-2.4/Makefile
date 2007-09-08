# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=brcm-2.4
BOARDNAME:=Broadcom BCM947xx/953xx
FEATURES:=squashfs

KERNEL:=2.4

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += kmod-switch kmod-diag nvram 

define Target/Description
	Build firmware images for Broadcom based routers
	(e.g. Linksys WRT54G(S), Asus WL-500g, Motorola WR850G)
endef

$(eval $(call BuildTarget))

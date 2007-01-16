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
	(e.g. Linksys WRT54G(S), Asus WL-500g, Motorola WR850G)
endef

include $(INCLUDE_DIR)/kernel-build.mk
DEFAULT_PACKAGES += kmod-switch kmod-diag

define Profile/Atheros
  NAME:=Atheros WiFi (default)
  PACKAGES:=kmod-madwifi
endef
$(eval $(call Profile,Atheros))

define Profile/None
  NAME:=No WiFi
  PACKAGES:=
endef
$(eval $(call Profile,None))

$(eval $(call BuildKernel))

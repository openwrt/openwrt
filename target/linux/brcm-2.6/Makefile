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
FEATURES:=squashfs jffs2

define Target/Description
	Build firmware images for Broadcom based routers
	(e.g. Linksys WRT54G(S), Asus WL-500g, Motorola WR850G)

	This one builds the generic .trx images, activate one of the
	options below for creating device specific files for use
	with TFTP client too.
endef

LINUX_VERSION:=2.6.17
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=37ddefe96625502161f075b9d907f21e

include $(INCLUDE_DIR)/kernel-build.mk
DEFAULT_PACKAGES += kmod-switch

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

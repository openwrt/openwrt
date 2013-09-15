#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=brcm47xx
BOARDNAME:=Broadcom BCM947xx/953xx
FEATURES:=squashfs usb pcmcia
MAINTAINER:=Hauke Mehrtens <hauke@hauke-m.de>

LINUX_VERSION:=3.10.12

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += swconfig kmod-diag wpad-mini nvram

define Target/Description
	Build firmware images for Broadcom based routers
	(e.g. Netgear WGT634U)
endef

$(eval $(call BuildTarget))

#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=brcm63xx
BOARDNAME:=Broadcom BCM63xx
FEATURES:=squashfs jffs2 usb atm pci pcmcia
LINUX_VERSION:=3.6.6
MAINTAINER:=Florian Fainelli <florian@openwrt.org>

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += kmod-switch kmod-input-gpio-keys-polled \
	kmod-button-hotplug

define Target/Description
	Build firmware images for Broadcom based xDSL/routers
	currently supports BCM6338, BCM6348 and BCM6358 based devices.
	(e.g. Inventel Livebox, Siemens SE515, Neufbox 4)
endef

$(eval $(call BuildTarget))

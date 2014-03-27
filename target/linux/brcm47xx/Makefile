#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=brcm47xx
BOARDNAME:=Broadcom BCM47xx/53xx (MIPS)
FEATURES:=squashfs usb pcmcia
MAINTAINER:=Hauke Mehrtens <hauke@hauke-m.de>

LINUX_VERSION:=3.10.34

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += swconfig wpad-mini nvram \
	kmod-leds-gpio kmod-gpio-button-hotplug \
	kmod-ledtrig-default-on kmod-ledtrig-timer kmod-ledtrig-netdev

define Target/Description
	Build firmware images for Broadcom based routers
	(e.g. Netgear WGT634U)
endef

$(eval $(call BuildTarget))

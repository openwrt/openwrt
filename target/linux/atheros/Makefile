#
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=atheros
BOARDNAME:=Atheros AR231x/AR5312
FEATURES:=squashfs

LINUX_VERSION:=3.10.28

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += wpad-mini kmod-ath5k swconfig kmod-gpio-button-hotplug

define Target/Description
	Build firmware images for Atheros SoC boards
endef

$(eval $(call BuildTarget))

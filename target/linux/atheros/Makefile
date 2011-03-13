#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=atheros
BOARDNAME:=Atheros AR231x/AR5312
FEATURES:=squashfs jffs2

LINUX_VERSION:=2.6.37.3

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += wpad-mini kmod-madwifi gpioctl swconfig

define Target/Description
	Build firmware images for Atheros SoC boards
endef

$(eval $(call BuildTarget))

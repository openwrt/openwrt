#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=adm8668
BOARDNAME:=Infineon WildPass ADM8668
FEATURES:=squashfs
MAINTAINER:=Florian Fainelli <florian@openwrt.org>

LINUX_VERSION:=2.6.36.4

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += wpad-mini kmod-rt61-pci
# kmod-switch kmod-diag nvram

define Target/Description
	Build firmware images for Infineon WildPass (ADM8668) based routers
	(e.g. T-Mobile branded Linksys WRTU54G-TM)
endef

$(eval $(call BuildTarget))

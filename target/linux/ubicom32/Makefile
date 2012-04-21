#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

#Add target specific (nommu) flags
TARGET_CFLAGS+= -DNO_FORK

ARCH:=ubicom32
BOARD:=ubicom32
BOARDNAME:=Ubicom IPxx
FEATURES:=nommu ramdisk broken

LINUX_VERSION:=2.6.30.10

include $(INCLUDE_DIR)/target.mk

KERNEL_MAKEOPTS += LDFLAGS_BUILD_ID=""

define Target/Description
	Build firmware images for Ubicom based routers and media devices.
endef

$(eval $(call BuildTarget))

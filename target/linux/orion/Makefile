#
# Copyright (C) 2008-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=orion
BOARDNAME:=Marvell Orion
SUBTARGETS:=generic harddisk
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

LINUX_VERSION:=3.10.44

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="zImage"

DEFAULT_PACKAGES += kmod-ath9k kmod-crypto-mv-cesa wpad-mini kmod-i2c-core kmod-i2c-mv64xxx kmod-rtc-isl1208

$(eval $(call BuildTarget))

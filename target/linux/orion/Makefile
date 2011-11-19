#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=orion
BOARDNAME:=Marvell Orion
SUBTARGETS:=generic harddisk
CFLAGS=-Os -pipe -march=armv5te -mtune=xscale -fno-caller-saves
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

LINUX_VERSION:=3.0.9

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="uImage"

DEFAULT_PACKAGES += kmod-ath9k kmod-crypto-mv-cesa wpad-mini

$(eval $(call BuildTarget))

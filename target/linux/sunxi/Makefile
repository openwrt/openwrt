#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=sunxi
BOARDNAME:=Allwinner SoC
FEATURES:=targz
SUBTARGETS=cortex-a7 cortex-a8
MAINTAINER:=Luka Perkov <luka@openwrt.org>

LINUX_VERSION:=3.10.17

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="zImage dtbs"

$(eval $(call BuildTarget))

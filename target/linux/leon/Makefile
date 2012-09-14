#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=sparc
BOARD:=leon
BOARDNAME:=LEON
FEATURES+=fpu tgz broken

CFLAGS:=-Os -pipe -mcpu=v8 -fno-caller-saves
KERNELNAME:="uImage"

LINUX_VERSION:=2.6.36.4
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

DEVICE_TYPE:=developerboard

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))

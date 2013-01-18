#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=mpc83xx
BOARDNAME:=Freescale MPC83xx
CFLAGS:=-Os -pipe -fno-caller-saves -mcpu=603e
FEATURES:=squashfs targz
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

LINUX_VERSION:=3.7.3

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += kmod-via-velocity

define Target/Description
	Build firmware images for Freescale MPC83xx based boards (eg. RouterBoard 600).
endef

$(eval $(call BuildTarget))

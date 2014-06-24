#
# Copyright (C) 2007-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=mpc83xx
BOARDNAME:=Freescale MPC83xx
CPU_TYPE:=603e
FEATURES:=squashfs targz
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

LINUX_VERSION:=3.8.13

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += kmod-via-velocity

define Target/Description
	Build firmware images for Freescale MPC83xx based boards (eg. RouterBoard 600).
endef

KERNELNAME:="uImage dtbImage.rb600 dtbImage.rb333"

$(eval $(call BuildTarget))

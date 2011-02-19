#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=mpc85xx
BOARDNAME:=Freescale MPC85xx
CFLAGS:=-Os -pipe -funit-at-a-time -mcpu=8540
FEATURES:=spe_fpu ramdisk broken
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

LINUX_VERSION:=2.6.36.4

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build images for the Freescale MPC85xx based boards.
endef

$(eval $(call BuildTarget))

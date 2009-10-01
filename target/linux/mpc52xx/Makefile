#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=mpc52xx
BOARDNAME:=Freescale MPC52xx
FEATURES:=tgz ext2

LINUX_VERSION:=2.6.30.8
LINUX_KARCH:=powerpc

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build images for the Freescale MPC52xx based boards.
endef

$(eval $(call BuildTarget))

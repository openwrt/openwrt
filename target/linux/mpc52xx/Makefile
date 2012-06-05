#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=mpc52xx
BOARDNAME:=Freescale MPC52xx
CFLAGS:=-Os -pipe -fno-caller-saves -mcpu=603e
FEATURES:=targz ext4

LINUX_VERSION:=3.3.8

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build images for the Freescale MPC52xx based boards.
endef

$(eval $(call BuildTarget))

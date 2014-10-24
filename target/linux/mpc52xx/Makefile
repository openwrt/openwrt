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
CPU_TYPE:=603e
FEATURES:=targz ext4

KERNEL_PATCHVER:=3.8

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build images for the Freescale MPC52xx based boards.
endef

KERNELNAME:=zImage

$(eval $(call BuildTarget))

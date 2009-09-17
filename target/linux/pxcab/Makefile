#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=pxcab
BOARDNAME:=PowerXCell Accelerator Board
FEATURES+=powerpc64 fpu ramdisk

LINUX_VERSION:=2.6.30.7

KERNEL_CC:=

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build zImage for the IBM PowerXCell Accelerator Board
	(aka mvXCell-8i from MatrixVision or GigaAccel 180 from Fixstars)
endef

KERNELNAME:="zImage"

DEFAULT_PACKAGES += kmod-tg3

$(eval $(call BuildTarget))

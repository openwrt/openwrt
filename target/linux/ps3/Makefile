#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=ps3
BOARDNAME:=Sony PS3 Game Console
MAINTAINER:=Geoff Levand <geoffrey.levand@am.sony.com>
SUBTARGETS=petitboot
FEATURES+=powerpc64

LINUX_VERSION:=2.6.30.9

KERNEL_CC:=

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build for the Sony PS3 Game Console

	The OpenWRT build will create both a 1st stage PS3-Linux image
	suitable for programming into the PS3 flash memory, and a 2nd
	stage PS3-Linux image suitable for loading via bootloaders or
	the kexec utility.

	Enable the Petitboot package to build a PS3 bootloader.
endef

KERNELNAME:="zImage"

$(eval $(call BuildTarget))

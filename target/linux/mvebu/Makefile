#
# Copyright (C) 2012-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=mvebu
BOARDNAME:=Marvell Armada XP/370
FEATURES:=targz usb pci pcie gpio
CPU_TYPE:=cortex-a9
CPU_SUBTYPE:=vfpv3

LINUX_VERSION:=3.10.12

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="zImage dtbs"

DEFAULT_PACKAGES +=

$(eval $(call BuildTarget))

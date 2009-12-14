# 
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=beagleboard
BOARDNAME:=beagleboard
FEATURES:=ext2 usb broken

LINUX_VERSION:=2.6.32-rc5
KERNELNAME:="uImage"

DEVICE_TYPE=developerboard

define Target/Description
	beagleboard - www.beagleboard.org - omap3 arm
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))

# 
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=omap35xx
BOARDNAME:=TI OMAP35xx
FEATURES:=broken
SUBTARGETS=generic

LINUX_VERSION:=2.6.32-rc5
KERNELNAME:="uImage"

DEVICE_TYPE=developerboard

define Target/Description
	omap35xx arm
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))

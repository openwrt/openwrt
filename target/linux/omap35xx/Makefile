#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=omap35xx
BOARDNAME:=TI OMAP35xx
FEATURES:=ubifs targz broken
SUBTARGETS=beagleboard gumstix

LINUX_VERSION:=2.6.32.33
KERNELNAME:="uImage"

DEVICE_TYPE=developerboard

define Target/Description
	omap35xx arm
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))

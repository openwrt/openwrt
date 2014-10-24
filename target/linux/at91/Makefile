# 
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=at91
MAINTAINER:=Claudio Mignanti <c.mignanti@gmail.com>
BOARDNAME:=Atmel AT91
FEATURES:=squashfs targz ext2 usb
SUBTARGETS:=legacy sama5d3

KERNEL_PATCHVER:=3.14

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += kmod-usb-ohci

$(eval $(call BuildTarget))

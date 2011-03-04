#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=omap24xx
BOARDNAME:=TI OMAP-24xx
MAINTAINER:=Michael Buesch <mb@bu3sch.de>
FEATURES:=targz squashfs jffs2 usb usbgadget display gpio audio

LINUX_VERSION:=2.6.38-rc7

define Target/Description
	TI OMAP-24xx
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))

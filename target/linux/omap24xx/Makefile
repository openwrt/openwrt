#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=omap24xx
BOARDNAME:=TI OMAP-24xx
FEATURES:=targz squashfs jffs2 usb usbgadget display gpio audio

LINUX_VERSION:=2.6.35.4

define Target/Description
	TI OMAP-24xx
endef

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))

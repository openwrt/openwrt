# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=at91
BOARDNAME:=Atmel AT91
FEATURES:=squashfs usb broken

LINUX_VERSION:=2.6.25.20

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="uImage"

define Target/Description
  Build fimware images for Figment Design Labs VersaLink board.
endef

$(eval $(call BuildTarget))

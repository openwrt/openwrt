#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=avr32
BOARD:=avr32
BOARDNAME:=Atmel AVR32
FEATURES:=squashfs
MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>

LINUX_VERSION:=2.6.39.4

include $(INCLUDE_DIR)/target.mk

define Target/Description
  Build firmware images for ATNGW100 board
endef

$(eval $(call BuildTarget))

# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=avr32
BOARD:=avr32
BOARDNAME:=Atmel AVR32
FEATURES:=squashfs

LINUX_VERSION:=2.6.22.1

define Target/Description
  Build firmware images for ATNGW100 board
endef

KERNEL:=2.6

include $(INCLUDE_DIR)/kernel-build.mk

#include the profiles
-include profiles/*.mk

$(eval $(call BuildKernel))

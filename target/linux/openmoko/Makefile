# 
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=openmoko
BOARDNAME:=Openmoko
FEATURES:=squashfs usb

LINUX_VERSION:=2.6.24.7

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="uImage"

define Target/Description
  Build fimware images for the Openmoko Smartphone.
endef

$(eval $(call BuildTarget))

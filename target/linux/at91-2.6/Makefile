# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=at91
BOARDNAME:=AT91
FEATURES:=squashfs

define Target/Description
  Build fimware images for Figment Design Labs VersaLink board.
endef

include $(INCLUDE_DIR)/kernel-build.mk

#include the profiles
-include profiles/*.mk

KERNELNAME:="uImage"
$(eval $(call BuildKernel))

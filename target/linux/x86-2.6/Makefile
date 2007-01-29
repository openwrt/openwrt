# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH=i386
BOARD:=x86
BOARDNAME:=x86
FEATURES:=jffs2 ext2

include $(INCLUDE_DIR)/kernel-build.mk
DEFAULT_PACKAGES += kmod-natsemi

$(eval $(call BuildKernel))


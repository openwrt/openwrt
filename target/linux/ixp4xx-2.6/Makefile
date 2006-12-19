# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=armeb
BOARD:=ixp4xx
BOARDNAME:=Intel XScale IXP4xx
FEATURES:=jffs2

define Target/Description
endef

LINUX_VERSION:=2.6.19.1
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=2ab08fdfddc00e09b3d5bc7397d3c8be

include $(INCLUDE_DIR)/kernel-build.mk
$(eval $(call BuildKernel))

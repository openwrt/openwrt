# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=i386
BOARD:=rdc
BOARDNAME:=RDC x86
FEATURES:=broken jffs2

define Target/Description
        Build firmware images for RDC3211 based routers
        (e.g. Airlink101 AR525W)
endef

LINUX_VERSION:=2.6.17
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=37ddefe96625502161f075b9d907f21e

include $(INCLUDE_DIR)/kernel-build.mk
$(eval $(call BuildKernel))

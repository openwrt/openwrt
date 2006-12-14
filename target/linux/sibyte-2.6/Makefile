# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=sibyte
BOARDNAME:=SiByte MIPS
FEATURES:=broken

LINUX_VERSION:=2.6.19
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=443c265b57e87eadc0c677c3acc37e20

include $(INCLUDE_DIR)/kernel-build.mk
$(eval $(call BuildKernel))

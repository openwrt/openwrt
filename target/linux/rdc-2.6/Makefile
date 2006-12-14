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

LINUX_VERSION:=2.6.19
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=443c265b57e87eadc0c677c3acc37e20

include $(INCLUDE_DIR)/kernel-build.mk

define Profile/Ralink
  NAME:=Ralink WiFi (default)
  PACKAGES:=kmod-rt61
endef
$(eval $(call Profile,Ralink))

$(eval $(call BuildKernel))

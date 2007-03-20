#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=romboot
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)
PKG_SOURCE:=$(PKG_NAME).tar.gz
PKG_SOURCE_URL:=http://www.teest.com/at91
PKG_MD5SUM:=
PKG_CAT:=zcat

include $(INCLUDE_DIR)/package.mk

define Build/InstallDev
	dd if=$(PKG_BUILD_DIR)/romboot.bin of=$(PKG_BUILD_DIR)/romboot.block bs=32k count=1 conv=sync
endef

$(eval $(call Build/DefaultTargets))

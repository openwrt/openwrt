#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=u-boot
PKG_VERSION:=1.1.5
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.denx.de/pub/u-boot
PKG_MD5SUM:=579707c8ecbf1ab4127285d2aac2a9ee
PKG_CAT:=bzcat

include $(INCLUDE_DIR)/package.mk

define Build/Compile
	cp -r ./files/* $(PKG_BUILD_DIR)
	cd $(PKG_BUILD_DIR);chmod a+x build_danube.sh;./build_danube.sh
endef

define Build/InstallDev
	dd if=$(PKG_BUILD_DIR)/u-boot.ifx of=$(BIN_DIR)/u-boot.ifx bs=64k conv=sync
endef

$(eval $(call Build/DefaultTargets))

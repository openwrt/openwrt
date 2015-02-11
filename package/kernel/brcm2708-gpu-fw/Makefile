# 
# Copyright (C) 2012-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=brcm2708-gpu-fw-boot
PKG_VERSION:=2015-02-07
PKG_RELEASE:=$(PKG_SOURCE_VERSION)
PKG_MD5SUM:=f198d5466ec412424f3ca77703f29d15

PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources/
# PKG_SOURCE_SUBDIR:=$(PKG_NAME)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/brcm2708-gpu-fw
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_brcm2708
  TITLE:=brcm2708-gpu-fw
  DEFAULT:=y if (TARGET_brcm2708)
endef

define Package/brcm2708-gpu-fw/description
 GPU and kernel boot firmware for brcm2708. 
endef

define Build/Compile
endef

$(eval $(call BuildPackage,brcm2708-gpu-fw))


# 
# Copyright (C) 2012-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=brcm2708-gpu-fw
PKG_REV:=38aa676b044f8de46aedb4bd972538a7ad6a3ce1
PKG_VERSION:=20150210
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_REV).tar.gz
PKG_SOURCE_URL:=https://github.com/Hexxeh/rpi-firmware/archive/
PKG_MD5SUM:=f5683c1dcb255714942f7c9fd61b3a0a

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)/rpi-firmware-$(PKG_REV)

include $(INCLUDE_DIR)/package.mk

define Package/brcm2708-gpu-fw
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@(TARGET_brcm2708_bcm2708||TARGET_brcm2708_bcm2709)
  TITLE:=brcm2708-gpu-fw
  DEFAULT:=y if (TARGET_brcm2708_bcm2708||TARGET_brcm2708_bcm2709)
endef

define Package/brcm2708-gpu-fw/description
 GPU and kernel boot firmware for brcm2708. 
endef

define Build/Compile
endef

define Build/InstallDev
	$(INSTALL_DIR) $(BUILD_DIR)/brcm2708-gpu-fw-boot
	$(CP) $(PKG_BUILD_DIR)/* $(BUILD_DIR)/brcm2708-gpu-fw-boot
endef

$(eval $(call BuildPackage,brcm2708-gpu-fw))


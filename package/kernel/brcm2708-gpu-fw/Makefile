# 
# Copyright (C) 2012-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=brcm2708-gpu-fw
PKG_REV:=94e1b8014dfa69701d0432088acb11ae01130d38
PKG_VERSION:=20140226
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_REV).tar.gz
PKG_SOURCE_URL:=https://github.com/Hexxeh/rpi-firmware/archive/
PKG_MD5SUM:=41fab917f6cda3ea74f127293ed1e8ce

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)/rpi-firmware-$(PKG_REV)

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

define Build/InstallDev
	$(INSTALL_DIR) $(BUILD_DIR)/brcm2708-gpu-fw-boot
	$(CP) $(PKG_BUILD_DIR)/* $(BUILD_DIR)/brcm2708-gpu-fw-boot
endef

$(eval $(call BuildPackage,brcm2708-gpu-fw))


#
# Copyright (C) 2020 Tobias Maedel <openwrt@tbspace.de>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=arm-trusted-firmware-rockchip-legacy
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/radxa/rkbin.git
PKG_SOURCE_DATE:=2022-11-12
PKG_SOURCE_VERSION:=9840e87723eef7c41235b89af8c049c1bcd3d133
PKG_HASH:=e1c4cd65d231741f22e291af7dc85ee262450fa84f3fb4b5d90cd9fa7b454f53

PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=license.md

PKG_MAINTAINER:=Han Pengfei <pengphei@qq.com>

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/trusted-firmware-a.mk
include $(INCLUDE_DIR)/package.mk

define Package/arm-trusted-firmware-rockchip-legacy
    SECTION:=boot
    CATEGORY:=Boot Loaders
    TITLE:=Legacy ARM Trusted Firmware for Rockchip
    DEPENDS:=@TARGET_rockchip_armv8
endef

define Build/Prepare
  $(TAR) -C $(PKG_BUILD_DIR) -xf $(DL_DIR)/$(PKG_SOURCE) ${PKG_NAME}-${PKG_VERSION}/bin
endef

define Build/Compile
endef

define Build/InstallDev
	$(INSTALL_DIR) -p $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/${PKG_NAME}-${PKG_VERSION}/bin/* $(STAGING_DIR_IMAGE)/
endef

define Package/arm-trusted-firmware-rockchip-legacy/install
endef

$(eval $(call BuildPackage,arm-trusted-firmware-rockchip-legacy))

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
PKG_SOURCE_URL=https://github.com/pengphei/rockchip-rkbin.git
PKG_SOURCE_DATE:=2022-11-12
PKG_SOURCE_VERSION:=24337a0f0729b4c8d10bd6103f55c31df9384e84
PKG_HASH:=dce4f1b76b0cdc9a3accdbe0367e7b0b0e15eedf71ee84416d461a211c497a38

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

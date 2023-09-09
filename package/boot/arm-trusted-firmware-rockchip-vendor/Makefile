# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 Tahomasoft.com
#
#
# github repo below is a static copy (July 2023) from 
# https://github.com/rockchip-linux/rkbin.git
#


include $(TOPDIR)/rules.mk


PKG_NAME:=arm-trusted-firmware-rockchip-vendor

PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/TahomaSoft/rkbin.git


PKG_MAINTAINER:=Erik Beck <bacon@tahomasoft.com>

MAKE_PATH:=$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/arm-trusted-firmware-rockchip-vendor
    SECTION:=boot
    CATEGORY:=Boot Loaders
    TITLE:=ARM Trusted Firmware for Rockchip
endef


define Package/arm-trusted-firmware-rk3568
    $(Package/arm-trusted-firmware-rockchip-vendor)
    DEPENDS:=@TARGET_rockchip_armv8
    VARIANT:=rk3568
endef


define Build/Configure
	$(SED) 's,$$$$(PKG_BUILD_DIR),$(PKG_BUILD_DIR),g' $(PKG_BUILD_DIR)/trust.ini
	$(SED) 's,$$$$(VARIANT),$(BUILD_VARIANT),g' $(PKG_BUILD_DIR)/trust.ini
	$(call Build/Configure/Default)
endef

define Build/Compile
	$(CURDIR)/pack-firmware.sh build $(BUILD_VARIANT) '$(PKG_BUILD_DIR)'
endef

define Build/InstallDev
	$(CURDIR)/pack-firmware.sh install $(BUILD_VARIANT) '$(PKG_BUILD_DIR)' '$(STAGING_DIR_IMAGE)'
endef


define Package/arm-trusted-firmware-rk3568/install
endef


$(eval $(call BuildPackage,arm-trusted-firmware-rk3568))


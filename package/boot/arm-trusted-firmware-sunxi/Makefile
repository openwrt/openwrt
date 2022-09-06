#
# Copyright (C) 2017 Hauke Mehrtens
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=arm-trusted-firmware-sunxi
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/ARM-software/arm-trusted-firmware
PKG_SOURCE_DATE:=2022-06-01
PKG_SOURCE_VERSION:=35f4c7295bafeb32c8bcbdfb6a3f2e74a57e732b
PKG_MIRROR_HASH:=88a282242ca5c921ce43eb913112e964ae87ddf85a87f2d4d6d192d1fe943370

PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=license.md

PKG_MAINTAINER:=Hauke Mehrtens <hauke@hauke-m.de>

include $(INCLUDE_DIR)/trusted-firmware-a.mk
include $(INCLUDE_DIR)/package.mk


define Package/arm-trusted-firmware-sunxi/Default
    SECTION:=boot
    CATEGORY:=Boot Loaders
    TITLE:=ARM Trusted Firmware for Allwinner
    DEPENDS:=@TARGET_sunxi_cortexa53
endef

define Package/arm-trusted-firmware-sunxi-a64
    $(call Package/arm-trusted-firmware-sunxi/Default)
    VARIANT:=sun50i_a64
endef

define Package/arm-trusted-firmware-sunxi-h6
    $(call Package/arm-trusted-firmware-sunxi/Default)
    VARIANT:=sun50i_h6
endef

define Package/arm-trusted-firmware-sunxi-h616
    $(call Package/arm-trusted-firmware-sunxi/Default)
    VARIANT:=sun50i_h616
endef

export GCC_HONOUR_COPTS=s

MAKE_VARS = \
	CROSS_COMPILE="$(TARGET_CROSS)"

MAKE_FLAGS += \
	PLAT=$(BUILD_VARIANT) \
	bl31

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/build/$(BUILD_VARIANT)/release/bl31.bin $(STAGING_DIR_IMAGE)/bl31_$(BUILD_VARIANT).bin
endef

define Package/arm-trusted-firmware-sunxi/install
endef

$(eval $(call BuildPackage,arm-trusted-firmware-sunxi-a64))
$(eval $(call BuildPackage,arm-trusted-firmware-sunxi-h6))
$(eval $(call BuildPackage,arm-trusted-firmware-sunxi-h616))

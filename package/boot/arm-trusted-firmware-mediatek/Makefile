#
# Copyright (C) 2017 Hauke Mehrtens
# Copyright (C) 2020 Daniel Golle
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=arm-trusted-firmware-mediatek
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/mtk-openwrt/arm-trusted-firmware.git
PKG_SOURCE_DATE:=2020-11-09
PKG_SOURCE_VERSION:=03017334ccd8c0fac12e7db36749b95b9a7d745f
PKG_MIRROR_HASH:=b211b2f9143d4debc7ad8dc959cb606888af20af790855dd66c87e451b6a1bc7

PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=docs/license.rst

PKG_MAINTAINER:=Daniel Golle <daniel@makrotopia.org>

include $(INCLUDE_DIR)/package.mk

define Download/mt7622-header-emmc
  URL:=https://raw.githubusercontent.com/frank-w/BPI-R64-ATF/a36efa5f7435b8079479d13b562fedc0aa0d42f0
  URL_FILE:=header_emmc.bin
  FILE:=mt7622-header_emmc.bin
  HASH:=0a09c55d90c5fc375c59468a4331555f77cbc3e1dd107ca83b5b8c9625f38d8c
endef

define Download/mt7622-header-sdmmc
  URL:=https://raw.githubusercontent.com/frank-w/BPI-R64-ATF/a36efa5f7435b8079479d13b562fedc0aa0d42f0
  URL_FILE:=header_sdmmc.bin
  FILE:=mt7622-header_sdmmc.bin
  HASH:=242908c04e25289d25ba9fab61a1930425af173051c43d275d1ac9877d6accb1
endef

define Package/arm-trusted-firmware-mt7622/Default
  SECTION:=boot
  CATEGORY:=Boot Loaders
  TITLE:=ARM Trusted Firmware for MT7622
  DEPENDS:=@TARGET_mediatek_mt7622
endef

define Package/arm-trusted-firmware-mt7622-nor
  $(call Package/arm-trusted-firmware-mt7622/Default)
  VARIANT:=nor
endef

define Package/arm-trusted-firmware-mt7622-snand
  $(call Package/arm-trusted-firmware-mt7622/Default)
  VARIANT:=snand
endef

define Package/arm-trusted-firmware-mt7622-emmc
  $(call Package/arm-trusted-firmware-mt7622/Default)
  VARIANT:=emmc
endef

define Package/arm-trusted-firmware-mt7622-sdmmc
  $(call Package/arm-trusted-firmware-mt7622/Default)
  VARIANT:=sdmmc
endef

export GCC_HONOUR_COPTS=s

MAKE_VARS = \
	CROSS_COMPILE="$(TARGET_CROSS)"

MAKE_FLAGS += \
	PLAT=mt7622 \
	BOOT_DEVICE=$(BUILD_VARIANT) \
	all

define Build/Prepare
	$(call Build/Prepare/Default)
ifeq ($(BUILD_VARIANT),emmc)
	$(eval $(call Download,mt7622-header-emmc))
endif
ifeq ($(BUILD_VARIANT),sdmmc)
	$(eval $(call Download,mt7622-header-sdmmc))
endif
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/build/mt7622/release/bl2.bin $(STAGING_DIR_IMAGE)/bl2-$(BUILD_VARIANT).bin
	$(CP) $(PKG_BUILD_DIR)/build/mt7622/release/bl2.img $(STAGING_DIR_IMAGE)/bl2-$(BUILD_VARIANT).img
# bl31.bin turns out to be identical for all build variants
	$(CP) $(PKG_BUILD_DIR)/build/mt7622/release/bl31.bin $(STAGING_DIR_IMAGE)/bl31.bin
ifeq ($(BUILD_VARIANT),emmc)
	$(CP) $(DL_DIR)/mt7622-header_emmc.bin $(STAGING_DIR_IMAGE)/header-emmc.bin
endif
ifeq ($(BUILD_VARIANT),sdmmc)
	$(CP) $(DL_DIR)/mt7622-header_sdmmc.bin $(STAGING_DIR_IMAGE)/header-sdmmc.bin
endif
endef

define Package/arm-trusted-firmware-mt7622-nor/install
endef
Package/arm-trusted-firmware-mt7622-snand/install = $(Package/arm-trusted-firmware-mt7622-nor/install)
Package/arm-trusted-firmware-mt7622-emmc/install = $(Package/arm-trusted-firmware-mt7622-nor/install)
Package/arm-trusted-firmware-mt7622-sdmmc/install = $(Package/arm-trusted-firmware-mt7622-nor/install)

$(eval $(call BuildPackage,arm-trusted-firmware-mt7622-nor))
$(eval $(call BuildPackage,arm-trusted-firmware-mt7622-snand))
$(eval $(call BuildPackage,arm-trusted-firmware-mt7622-emmc))
$(eval $(call BuildPackage,arm-trusted-firmware-mt7622-sdmmc))

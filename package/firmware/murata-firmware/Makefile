#
# Copyright (C) 2024 Bootlin
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=murata-firmware
PKG_VERSION:=2020.09.14
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/murata-wireless/cyw-fmac-fw.git
PKG_MIRROR_HASH:=c2fc14802860f923d7bea36a709efeeffce4637a57084d78133e080b3dd8a87c
PKG_SOURCE_VERSION:=52174a18134c7ef4a674ecd9fb68fc6e2bced969

PKG_MAINTAINER:=Thomas Richard <thomas.richard@bootlin.com>
PKG_LICENSE_FILES:=LICENCE

include $(INCLUDE_DIR)/package.mk

define Package/murata-firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://community.murata.com/
endef

define Build/Compile
endef

define Package/murata-firmware-43430-sdio
  $(Package/murata-firmware-default)
  TITLE:=BCM43430 FullMac SDIO firmware
  CONFLICTS:=brcmfmac-firmware-43430-sdio
endef

define Package/murata-firmware-43430-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/murata
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cyfmac43430-sdio.bin \
		$(1)/lib/firmware/murata/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cyfmac43430-sdio.1DX.clm_blob \
		$(1)/lib/firmware/murata/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../murata/cyfmac43430-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.bin
	$(LN) \
		../murata/cyfmac43430-sdio.1DX.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.clm_blob
endef

$(eval $(call BuildPackage,murata-firmware-43430-sdio))


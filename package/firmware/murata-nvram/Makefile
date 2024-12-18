#
# Copyright (C) 2024 Bootlin
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=murata-nvram
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2024-09-28
PKG_SOURCE_URL:=https://github.com/murata-wireless/cyw-fmac-nvram.git
PKG_MIRROR_HASH:=e2fc6f1f9898edfd67f5495e615882700f03b42a75efa2e3f4e2d524aed0efb8
PKG_SOURCE_VERSION:=255819aab07572d87576a13329ef9d4c654167aa

PKG_MAINTAINER:=Thomas Richard <thomas.richard@bootlin.com>
PKG_LICENSE_FILES:=LICENCE.cypress

include $(INCLUDE_DIR)/package.mk

define Package/murata-nvram-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://community.murata.com/
endef

define Build/Compile
endef

define Package/murata-nvram-43430-sdio
  $(Package/murata-nvram-default)
  TITLE:=BCM43430 SDIO NVRAM
  CONFLICTS:=brcmfmac-nvram-43430-sdio
endef

define Package/murata-nvram-43430-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA)	./files/cyfmac43430-sdio.1DX.stm32.txt \
		$(1)/lib/firmware/brcm/
	$(LN) \
		cyfmac43430-sdio.1DX.stm32.txt \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.st,stm32mp135f-dk.txt
endef

$(eval $(call BuildPackage,murata-nvram-43430-sdio))

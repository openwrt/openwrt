#
# Copyright (C) 2024 Bootlin
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=murata-nvram
PKG_VERSION:=2020.09.14
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/murata-wireless/cyw-fmac-nvram.git
PKG_MIRROR_HASH:=6dc361ab8cd93d5a8996e141e9558fa601e6ef5f99e64bc264e8b176dcd53bd7
PKG_SOURCE_VERSION:=45fe43ad51ad47a0c57ad307db3e87da766bf61e

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
	$(INSTALL_DIR) $(1)/lib/firmware/murata
	$(INSTALL_DATA)	$(PKG_BUILD_DIR)/cyfmac43430-sdio.1DX.txt \
		$(1)/lib/firmware/murata/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) ../murata/cyfmac43430-sdio.1DX.txt \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.txt
endef

$(eval $(call BuildPackage,murata-nvram-43430-sdio))

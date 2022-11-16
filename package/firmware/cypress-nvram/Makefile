# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=cypress-nvram
PKG_RELEASE:=5

PKG_MAINTAINER:=Álvaro Fernández Rojas <noltari@gmail.com>

include $(INCLUDE_DIR)/package.mk

define Package/cypress-nvram-default
  SECTION:=firmware
  CATEGORY:=Firmware
endef

define Build/Compile
	true
endef

# Cypress 4339 SDIO AP6335 NVRAM
define Package/cypress-nvram-4339-sdio-ap6335
  $(Package/cypress-nvram-default)
  TITLE:=BCM4339 NVRAM for AP6335
  CONFLICTS:=brcmfmac-firmware-4339-sdio-ap6335
endef

define Package/cypress-nvram-4339-sdio-ap6335/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		./files/brcmfmac4339-sdio.AP6335.txt \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,cypress-nvram-4339-sdio-ap6335))

# Cypress 4339 SDIO PICO-PI-IMX7D NVRAM
define Package/cypress-nvram-4339-sdio-pico-pi-imx7d
  $(Package/cypress-nvram-default)
  TITLE:=BCM4339 NVRAM for TechNexion PICO-PI-IMX7D
  DEPENDS:=+cypress-nvram-4339-sdio-ap6335
  CONFLICTS:=brcmfmac-firmware-4339-sdio-pico-pi-imx7d
endef

define Package/cypress-nvram-4339-sdio-pico-pi-imx7d/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		brcmfmac4339-sdio.AP6335.txt \
		$(1)/lib/firmware/brcm/brcmfmac4339-sdio.technexion,imx7d-pico-pi.txt
endef

$(eval $(call BuildPackage,cypress-nvram-4339-sdio-pico-pi-imx7d))

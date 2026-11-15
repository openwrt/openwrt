# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=cypress-firmware
PKG_VERSION_REAL:=6.1.145-2026_0407
PKG_VERSION:=$(subst _,.,$(subst -,.,$(PKG_VERSION_REAL)))
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/Infineon/ifx-linux-firmware/
PKG_MIRROR_HASH:=1f119e1a0d3c6c9ff5f2c07d73eb732dfdf16756e26e2408e3cd4322d7da46bd
PKG_SOURCE_VERSION:=release-v$(PKG_VERSION_REAL)

PKG_MAINTAINER:=Álvaro Fernández Rojas <noltari@gmail.com>
PKG_LICENSE:=LicenseRef-Cypress
PKG_LICENSE_FILES:=LICENCE

include $(INCLUDE_DIR)/package.mk

define Package/cypress-firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://community.infineon.com/
endef

define Build/Compile
	true
endef

# Cypress 43012 SDIO Firmware
define Package/cypress-firmware-43012-sdio
  $(Package/cypress-firmware-default)
  TITLE:=CYW43012 FullMac SDIO firmware
endef

define Package/cypress-firmware-43012-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac43012-sdio.bin \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac43012-sdio.clm_blob \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac43012-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac43012-sdio.bin
	$(LN) \
		../cypress/cyfmac43012-sdio.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac43012-sdio.clm_blob
endef

$(eval $(call BuildPackage,cypress-firmware-43012-sdio))

# Cypress 43439 SDIO Firmware
define Package/cypress-firmware-43439-sdio
  $(Package/cypress-firmware-default)
  TITLE:=CYW43439 FullMac SDIO firmware
endef

define Package/cypress-firmware-43439-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac43439-sdio.bin \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac43439-sdio.clm_blob \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac43439-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac43439-sdio.bin
	$(LN) \
		../cypress/cyfmac43439-sdio.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac43439-sdio.clm_blob
endef

$(eval $(call BuildPackage,cypress-firmware-43439-sdio))

# Cypress 43455 SDIO Firmware
define Package/cypress-firmware-43455-sdio
  $(Package/cypress-firmware-default)
  TITLE:=CYW43455 FullMac SDIO firmware
  PROVIDES:=brcmfmac-firmware-43455-sdio
  CONFLICTS:=brcmfmac-firmware-43455-sdio
endef

define Package/cypress-firmware-43455-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac43455-sdio.bin \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac43455-sdio.clm_blob \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac43455-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.bin
	$(LN) \
		../cypress/cyfmac43455-sdio.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.clm_blob
endef

$(eval $(call BuildPackage,cypress-firmware-43455-sdio))

# Cypress 4373 PCIe Firmware
define Package/cypress-firmware-4373-pcie
  $(Package/cypress-firmware-default)
  TITLE:=CYW4373 FullMac PCIe firmware
endef

define Package/cypress-firmware-4373-pcie/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac4373-pcie.bin \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac4373-pcie.clm_blob \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac4373-pcie.bin \
		$(1)/lib/firmware/brcm/brcmfmac4373-pcie.bin
	$(LN) \
		../cypress/cyfmac4373-pcie.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac4373-pcie.clm_blob
endef

$(eval $(call BuildPackage,cypress-firmware-4373-pcie))

# Cypress 4373 SDIO Firmware
define Package/cypress-firmware-4373-sdio
  $(Package/cypress-firmware-default)
  TITLE:=CYW4373 FullMac SDIO firmware
endef

define Package/cypress-firmware-4373-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac4373-sdio.bin \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac4373-sdio.clm_blob \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac4373-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac4373-sdio.bin
	$(LN) \
		../cypress/cyfmac4373-sdio.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac4373-sdio.clm_blob
endef

$(eval $(call BuildPackage,cypress-firmware-4373-sdio))

# Cypress 4373 USB Firmware
define Package/cypress-firmware-4373-usb
  $(Package/cypress-firmware-default)
  TITLE:=CYW4373 FullMac USB firmware
endef

define Package/cypress-firmware-4373-usb/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac4373-usb.bin \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac4373-usb.clm_blob \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac4373-usb.bin \
		$(1)/lib/firmware/brcm/brcmfmac4373-usb.bin
	$(LN) \
		../cypress/cyfmac4373-usb.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac4373.clm_blob
endef

$(eval $(call BuildPackage,cypress-firmware-4373-usb))

# Cypress 54591 PCIe Firmware
define Package/cypress-firmware-54591-pcie
  $(Package/cypress-firmware-default)
  TITLE:=CYW54591 FullMac PCIe firmware
endef

define Package/cypress-firmware-54591-pcie/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac54591-pcie.bin \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac54591-pcie.clm_blob \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac54591-pcie.bin \
		$(1)/lib/firmware/brcm/brcmfmac54591-pcie.bin
	$(LN) \
		../cypress/cyfmac54591-pcie.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac54591-pcie.clm_blob
endef

$(eval $(call BuildPackage,cypress-firmware-54591-pcie))

# Cypress 54591 SDIO Firmware
define Package/cypress-firmware-54591-sdio
  $(Package/cypress-firmware-default)
  TITLE:=CYW54591 FullMac SDIO firmware
endef

define Package/cypress-firmware-54591-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac54591-sdio.bin \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac54591-sdio.clm_blob \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac54591-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac54591-sdio.bin
	$(LN) \
		../cypress/cyfmac54591-sdio.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac54591-sdio.clm_blob
endef

$(eval $(call BuildPackage,cypress-firmware-54591-sdio))

# Cypress 55572 PCIe Firmware
define Package/cypress-firmware-55572-pcie
  $(Package/cypress-firmware-default)
  TITLE:=CYW55572 FullMac PCIe firmware
endef

define Package/cypress-firmware-55572-pcie/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac55572-pcie.trxse \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac55572-pcie.clm_blob \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac55572-pcie.trxse \
		$(1)/lib/firmware/brcm/brcmfmac55572-pcie.trxse
	$(LN) \
		../cypress/cyfmac55572-pcie.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac55572-pcie.clm_blob
endef

$(eval $(call BuildPackage,cypress-firmware-55572-pcie))

# Cypress 55572 SDIO Firmware
define Package/cypress-firmware-55572-sdio
  $(Package/cypress-firmware-default)
  TITLE:=CYW55572 FullMac SDIO firmware
endef

define Package/cypress-firmware-55572-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac55572-sdio.trxse \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmware/cyfmac55572-sdio.clm_blob \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac55572-sdio.trxse \
		$(1)/lib/firmware/brcm/brcmfmac55572-sdio.trxse
	$(LN) \
		../cypress/cyfmac55572-sdio.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac55572-sdio.clm_blob
endef

$(eval $(call BuildPackage,cypress-firmware-55572-sdio))

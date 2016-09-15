#
# Copyright (C) 2016 OpenWrt.org
# Copyright (C) 2016 LEDE project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=brcmfmac-firmware-43430-sdio
PKG_VERSION:=2016-06-07
PKG_RELEASE:=a7491de4c4b2f1ceb5d0dfa5350b95e5c6fb9bd4

include $(INCLUDE_DIR)/package.mk

BRCMFMAC43430_SDIO_URL:=@GITHUB/RPi-Distro/firmware-nonfree/$(PKG_RELEASE)/brcm80211/brcm/
BRCMFMAC43430_SDIO_FILE:=brcmfmac43430-sdio-$(PKG_RELEASE)

define Download/brcmfmac43430_sdio_bin
  FILE:=$(BRCMFMAC43430_SDIO_FILE).bin
  URL:=$(BRCMFMAC43430_SDIO_URL)
  URL_FILE:=brcmfmac43430-sdio.bin
  MD5SUM:=b24aad300e161782072b227abe9862f7
endef
$(eval $(call Download,brcmfmac43430_sdio_bin))

define Download/brcmfmac43430_sdio_txt
  FILE:=$(BRCMFMAC43430_SDIO_FILE).txt
  URL:=$(BRCMFMAC43430_SDIO_URL)
  URL_FILE:=brcmfmac43430-sdio.txt
  MD5SUM:=8c3cb6d8f0609b43f09d083b4006ec5a
endef
$(eval $(call Download,brcmfmac43430_sdio_txt))

define Package/brcmfmac-firmware-43430-sdio
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=Broadcom BCM43430 FullMac SDIO firmware
  URL:=http://www.broadcom.com/
endef

define Package/brcmfmac-firmware-43430-sdio/description
	Broadcom BCM43430 fullmac SDIO firmware
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	cp $(DL_DIR)/$(BRCMFMAC43430_SDIO_FILE).bin $(PKG_BUILD_DIR)/brcmfmac43430-sdio.bin
	cp $(DL_DIR)/$(BRCMFMAC43430_SDIO_FILE).txt $(PKG_BUILD_DIR)/brcmfmac43430-sdio.txt
endef

define Build/Compile
	true
endef

define Package/brcmfmac-firmware-43430-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/brcmfmac43430-sdio.bin $(1)/lib/firmware/brcm/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/brcmfmac43430-sdio.txt $(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,brcmfmac-firmware-43430-sdio))

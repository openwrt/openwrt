# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=brcmfmac43430-firmware-sdio
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

BRCMFMAC43430_SDIO_REV:=54bab3d6a6d43239c71d26464e6e10e5067ffea7
BRCMFMAC43430_SDIO_URL:=@GITHUB/RPi-Distro/firmware-nonfree/$(BRCMFMAC43430_SDIO_REV)/brcm80211/brcm/
BRCMFMAC43430_SDIO_FILE:=brcmfmac43430-sdio-$(BRCMFMAC43430_SDIO_REV)

define Download/brcmfmac43430_sdio_bin
  FILE:=$(BRCMFMAC43430_SDIO_FILE).bin
  URL:=$(BRCMFMAC43430_SDIO_URL)
  URL_FILE:=brcmfmac43430-sdio.bin
  MD5SUM:=4a410ab9a1eefe82e158d36df02b3589
endef
$(eval $(call Download,brcmfmac43430_sdio_bin))

define Download/brcmfmac43430_sdio_txt
  FILE:=$(BRCMFMAC43430_SDIO_FILE).txt
  URL:=$(BRCMFMAC43430_SDIO_URL)
  URL_FILE:=brcmfmac43430-sdio.txt
  MD5SUM:=8c3cb6d8f0609b43f09d083b4006ec5a
endef
$(eval $(call Download,brcmfmac43430_sdio_txt))

define Package/brcmfmac43430-firmware-sdio
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=Broadcom BCM43430 fullmac SDIO firmware
  URL:=http://www.broadcom.com/
endef

define Package/brcmfmac43430-firmware-sdio/description
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

define Package/brcmfmac43430-firmware-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/brcmfmac43430-sdio.bin $(1)/lib/firmware/brcm/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/brcmfmac43430-sdio.txt $(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,brcmfmac43430-firmware-sdio))

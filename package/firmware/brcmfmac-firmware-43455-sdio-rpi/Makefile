#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=brcmfmac-firmware-43455-sdio-rpi
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/RPi-Distro/firmware-nonfree.git
PKG_SOURCE_DATE:=2018-06-05
PKG_SOURCE_VERSION:=86e88fbf0345da49555d0ec34c80b4fbae7d0cd3
PKG_MIRROR_HASH:=b29d4701326f076b0b0f64bf5163bc86570b1ae1d6817794f796d9f294b2e5a6

PKG_MAINTAINER:=Robert Marko <robimarko@gmail.com>

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/brcmfmac-firmware-43455-sdio-rpi
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://github.com/RPi-Distro/firmware-nonfree.git
  TITLE:=Broadcom BCM43455 FullMac SDIO firmware
  DEPENDS:=@TARGET_brcm2708
endef

define Package/brcmfmac-firmware-43455-sdio-rpi/install 
	$(INSTALL_DIR) $(1)/lib/firmware/brcm 
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43455-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.bin
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43455-sdio.txt \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.txt
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43455-sdio.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.clm_blob
endef

$(eval $(call BuildPackage,brcmfmac-firmware-43455-sdio-rpi))

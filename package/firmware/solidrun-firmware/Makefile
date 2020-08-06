#
# Copyright (C) 2020 Josua Mayer <josua@solid-run.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=solidrun-firmware
PKG_SOURCE_DATE:=2019-09-05
PKG_SOURCE_VERSION:=3ff92e688187d39b694f4e5a5cefe9d5e366ea81
PKG_MIRROR_HASH:=2e504e071c3f896d629c4cfffe7ff4b5f1acdb4fecd3f01e8ff8c73e87a67cc7
PKG_RELEASE:=1
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/SolidRun/deb-pkg_cuboxi-firmware-wireless.git
PKG_MAINTAINER:=Josua Mayer <josua@solid-run.com>

include $(INCLUDE_DIR)/package.mk

define Package/brcmfmac-firmware-4329-sdio-cubox-i
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=
  TITLE:=BCM4329 NVRAM for Cubox-i
endef

define Package/brcmfmac-firmware-4330-sdio-cubox-i
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=
  TITLE:=BCM4330 NVRAM for Cubox-i
endef

define Package/wl18xx-firmware-cubox-i
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=
  TITLE:=WL1835 NVRAM for Cubox-i
endef

define Build/Compile

endef

define Package/brcmfmac-firmware-4329-sdio-cubox-i/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcmfmac4329-sdio.txt \
		$(1)/lib/firmware/brcm/brcmfmac4329-sdio.solidrun,cubox-i.txt
	ln -s brcmfmac4329-sdio.solidrun,cubox-i.txt $(1)/lib/firmware/brcm/brcmfmac4329-sdio.solidrun,cubox-i-dl.txt
	ln -s brcmfmac4329-sdio.solidrun,cubox-i.txt $(1)/lib/firmware/brcm/brcmfmac4329-sdio.solidrun,cubox-i-q.txt
	ln -s brcmfmac4329-sdio.solidrun,cubox-i.txt $(1)/lib/firmware/brcm/brcmfmac4329-sdio.solidrun,hummingboard-dl.txt
	ln -s brcmfmac4329-sdio.solidrun,cubox-i.txt $(1)/lib/firmware/brcm/brcmfmac4329-sdio.solidrun,hummingboard-q.txt
endef

define Package/brcmfmac-firmware-4330-sdio-cubox-i/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcmfmac4330-sdio.txt \
		$(1)/lib/firmware/brcm/brcmfmac4330-sdio.solidrun,cubox-i.txt
	ln -s brcmfmac4330-sdio.solidrun,cubox-i.txt $(1)/lib/firmware/brcm/brcmfmac4330-sdio.solidrun,cubox-i-dl.txt
	ln -s brcmfmac4330-sdio.solidrun,cubox-i.txt $(1)/lib/firmware/brcm/brcmfmac4330-sdio.solidrun,cubox-i-q.txt
	ln -s brcmfmac4330-sdio.solidrun,cubox-i.txt $(1)/lib/firmware/brcm/brcmfmac4330-sdio.solidrun,hummingboard-dl.txt
	ln -s brcmfmac4330-sdio.solidrun,cubox-i.txt $(1)/lib/firmware/brcm/brcmfmac4330-sdio.solidrun,hummingboard-q.txt
endef

define Package/wl18xx-firmware-cubox-i/install
	$(INSTALL_DIR) $(1)/lib/firmware/ti-connectivity
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/wl18xx-conf.bin \
		$(1)/lib/firmware/ti-connectivity/wl18xx-conf.bin
endef

$(eval $(call BuildPackage,brcmfmac-firmware-4329-sdio-cubox-i))
$(eval $(call BuildPackage,brcmfmac-firmware-4330-sdio-cubox-i))
$(eval $(call BuildPackage,wl18xx-firmware-cubox-i))

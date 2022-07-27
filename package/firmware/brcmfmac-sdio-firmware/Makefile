#
# Copyright (C) 2019-2020 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=brcmfmac-sdio-firmware
PKG_SOURCE_DATE:=2022-07-06
PKG_SOURCE_VERSION:=afc477e807c407736cfaff6a6188d09197dfbceb
PKG_MIRROR_HASH:=3272b797f72d6b147d01cb82b72fb755ac0231ec4286b9571825c14e324baecd
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/LibreELEC/brcmfmac_sdio-firmware

PKG_MAINTAINER:=Florian Fainelli <f.fainelli@gmail.com>
PKG_LICENSE:=Proprietary
PKG_LICENSE_FILES:=LICENCE.broadcom_bcm43xx

include $(INCLUDE_DIR)/package.mk

define Package/brcmfmac-sdio-firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://github.com/LibreELEC/brcmfmac_sdio-firmware
endef

define Build/Compile
	true
endef

define Package/brcmfmac-sdio-firmware-43430-bt
  $(Package/brcmfmac-sdio-firmware-default)
  TITLE:=CYW43430 BT firmware and patch RAM
endef

define Package/brcmfmac-sdio-firmware-43430-bt/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/BCM43430*.hcd \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,brcmfmac-sdio-firmware-43430-bt))

define Package/brcmfmac-sdio-firmware-4345-bt
  $(Package/brcmfmac-sdio-firmware-default)
  TITLE:=CYW4345 BT firmware and patch RAM
endef

define Package/brcmfmac-sdio-firmware-4345-bt/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/BCM4345*.hcd \
		$(1)/lib/firmware/brcm/
endef

$(eval $(call BuildPackage,brcmfmac-sdio-firmware-4345-bt))

# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=rtl8192su-firmware
PKG_SOURCE_DATE:=2017-03-31
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/chunkeey/rtl8192su.git
PKG_SOURCE_VERSION:=1f09c7a6eceffc92e63ddc636c174e8f3770eeb9
PKG_MIRROR_HASH:=dca07ce948db9b51bc327d424a6a0eee8fd19b0208aeb1822ee8edc707093aa1

PKG_MAINTAINER:=Dmitry R <rdmitry0911@gmail.com>
PKG_LICENSE:=LicenseRef-Realtek-Firmware
PKG_LICENSE_FILES:=firmwares/LICENCE.rtlwifi_firmware.txt

include $(INCLUDE_DIR)/package.mk

define Package/rtl8192su-firmware
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=Realtek RTL8192SU/RTL8188SU AP firmware
  URL:=https://github.com/chunkeey/rtl8192su
  PKGARCH:=all
endef

define Build/Compile
	true
endef

define Package/rtl8192su-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/rtlwifi
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/firmwares/rtl8192sufw-ap.bin \
		$(1)/lib/firmware/rtlwifi/rtl8192sufw-ap.bin
endef

$(eval $(call BuildPackage,rtl8192su-firmware))

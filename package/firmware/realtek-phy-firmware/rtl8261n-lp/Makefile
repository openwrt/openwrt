# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=rtl8261n-lp-firmware
PKG_SOURCE:=rtl8261n_lp.bin
PKG_SOURCE_URL:=https://github.com/balika011/realtek_phy_firmware/releases/download/rtl8261n-lp-2024.03.05/
PKG_SOURCE_VERSION:=386e15fe6a117be8c32ea14b36456a3630d027dc
PKG_HASH:=ed151b2db26ebd4a7276d7b6f85d3a955a238a773e509418bf6213d1be01d887

PKG_MAINTAINER:=Balázs Triszka <info@balika011.hu>
PKG_LICENSE_FILES:=LICENCE

include $(INCLUDE_DIR)/package.mk

PKG_UNPACK:=cp $(DL_DIR)/$(PKG_SOURCE) $(PKG_BUILD_DIR)

define Build/Compile
	true
endef

define Package/rtl8261n-lp-firmware
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=Realtek RTL8251L/RTL8261BE/RTL8261N low-power firmware
  PROVIDES:=rtl8261n-firmware
  CONFLICTS:=rtl8261n-firmware
endef

define Package/rtl8261n-lp-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/rtl8261n_lp.bin \
		$(1)/lib/firmware/rtl8261n.bin
endef

$(eval $(call BuildPackage,rtl8261n-lp-firmware))


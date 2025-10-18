# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=rtl8261n-firmware
PKG_SOURCE:=rtl8261n.bin
PKG_SOURCE_URL:=https://github.com/balika011/realtek_phy_firmware/releases/download/rtl8261n-2024.03.05/
PKG_SOURCE_VERSION:=386e15fe6a117be8c32ea14b36456a3630d027dc
PKG_HASH:=3991ababa2de76f928ae5af1835f80534f183dc549dbd4f9eee419b7f2dea351

PKG_MAINTAINER:=Balázs Triszka <info@balika011.hu>
PKG_LICENSE_FILES:=LICENCE

include $(INCLUDE_DIR)/package.mk

PKG_UNPACK:=cp $(DL_DIR)/$(PKG_SOURCE) $(PKG_BUILD_DIR)

define Build/Compile
	true
endef

define Package/rtl8261n-firmware
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=Realtek RTL8251L/RTL8261BE/RTL8261N firmware
endef

define Package/rtl8261n-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/rtl8261n.bin \
		$(1)/lib/firmware/
endef

$(eval $(call BuildPackage,rtl8261n-firmware))


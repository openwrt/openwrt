# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=rtl8264b-firmware
PKG_SOURCE:=rtl8264b.bin
PKG_SOURCE_URL:=https://github.com/balika011/realtek_phy_firmware/releases/download/rtl8264b-2023.01.31/
PKG_SOURCE_VERSION:=386e15fe6a117be8c32ea14b36456a3630d027dc
PKG_HASH:=466bf162780fce13bc34ba23a2938b50421426dd3e646ae55ccac6c677ee3cb0

PKG_MAINTAINER:=Balázs Triszka <info@balika011.hu>
PKG_LICENSE_FILES:=LICENCE

include $(INCLUDE_DIR)/package.mk

PKG_UNPACK:=cp $(DL_DIR)/$(PKG_SOURCE) $(PKG_BUILD_DIR)

define Build/Compile
	true
endef

define Package/rtl8264b-firmware
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=Realtek RTL8254B/RTL8264/RTL8264B firmware
endef

define Package/rtl8264b-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/rtl8264b.bin \
		$(1)/lib/firmware/
endef

$(eval $(call BuildPackage,rtl8264b-firmware))


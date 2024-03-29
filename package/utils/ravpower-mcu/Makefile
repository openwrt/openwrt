include $(TOPDIR)/rules.mk

PKG_NAME:=ravpower-mcu
PKG_RELEASE:=2

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/blocktrron/ravpower-mcu.git
PKG_MIRROR_HASH:=b018291629782d46cd5e3aae59c18598f83d2bae2edd1e356b88d296be07a48a
PKG_SOURCE_DATE:=2020-06-19
PKG_SOURCE_VERSION:=1665d9e9212dcd118629a74fbe658841f81036f7
PKG_MAINTAINER:=David Bauer <mail@david-bauer.net>
PKG_LICENSE:=GPL-2.0-or-later

include $(INCLUDE_DIR)/package.mk

define Package/ravpower-mcu
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Utility to control the RAVPower RP-WD009 PMIC
  URL:=https://github.com/blocktrron/ravpower-mcu/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall"
endef

define Package/ravpower-mcu/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/out/ravpower-mcu $(1)/usr/bin/
endef

$(eval $(call BuildPackage,ravpower-mcu))

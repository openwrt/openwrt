include $(TOPDIR)/rules.mk

PKG_NAME:=dns320l-mcu
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/wigyori/dns320l-daemon.git
PKG_SOURCE_DATE:=2024-05-11
PKG_SOURCE_VERSION:=c74941880e0a8d2bc0344b3256b984397512e8c3
PKG_MIRROR_HASH:=e0f186a0c139ccfac3d311f49e2fecdbd02fa3f9fe6ced4b1ce0baa603d49fc3
PKG_MAINTAINER:=Zoltan HERPAI <wigyori@uid0.hu>
PKG_LICENSE:=GPL-3.0+

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/dns320l-mcu
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Utility to control the MCU on DNS-320L
  DEPENDS:=@TARGET_kirkwood
  URL:=https://github.com/wigyori/dns320l-mcu
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall"
endef

define Package/dns320l-mcu/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/init.d
	$(INSTALL_BIN) ./files/dns320l-mcu.init $(1)/etc/init.d/dns320l-mcu
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dns320l-daemon $(1)/usr/bin/dns320l-mcu
endef

$(eval $(call BuildPackage,dns320l-mcu))

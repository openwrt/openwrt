include $(TOPDIR)/rules.mk

PKG_NAME:=dns320l-mcu
PKG_RELEASE:=2

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/wigyori/dns320l-daemon.git
PKG_SOURCE_DATE:=2025-05-03
PKG_SOURCE_VERSION:=11fcf3bbc98cc1efc64479ffbea8fb86d91c57c2
PKG_MIRROR_HASH:=4e16dc098aeb5845b0aa977b6f34b67c549bea83e194bf841c9061fb9385fa5c
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

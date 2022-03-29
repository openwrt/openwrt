include $(TOPDIR)/rules.mk

PKG_NAME:=paho.mqtt.c
PKG_VERSION:=1.3.1
PKG_RELEASE:=1

PKG_SOURCE_URL:=https://github.com/eclipse/paho.mqtt.c/archive/
PKG_SOURCE:=v$(PKG_VERSION).zip
PKG_MD5SUM:=e53729db4585b41d0661065667503048

include $(INCLUDE_DIR)/package.mk

define Package/paho.mqtt.c
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE+=paho.mqtt.c for OpenWrt
  DEPENDS:=+libopenssl
endef

define Package/paho.mqtt.c/description
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		LINUX="$(LINUX_DIR)" \
		CC="$(TARGET_CC)" \
		STAGING_DIR="$(STAGING_DIR)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		CFLAGS="$(TARGET_CFLAGS)"
endef

define Package/paho.mqtt.c/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/output/*s.so.* $(1)/usr/lib/
endef


$(eval $(call BuildPackage,paho.mqtt.c))

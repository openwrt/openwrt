include $(TOPDIR)/rules.mk

PKG_NAME:=fancontrol
PKG_VERSION:=1
PKG_RELEASE:=3


PKG_LICENSE:=MIT
PKG_MAINTAINER:=JiaY-shi <shi05275@163.com>

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/fancontrol
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=FanControl for OpenWRT
  DEPENDS:=+kmod-hwmon-pwmfan
endef

define Package/fancontrol/description
  FanControl for OpenWRT.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

MAKE_FLAGS += \
	-C $(PKG_BUILD_DIR)

define Package/fancontrol/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fancontrol $(1)/usr/bin/fancontrol
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/fancontrol.config $(1)/etc/config/fancontrol
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/fancontrol.init $(1)/etc/init.d/fancontrol
endef

$(eval $(call BuildPackage,fancontrol))

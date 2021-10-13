include $(TOPDIR)/rules.mk

PKG_NAME:=ubtled
PKG_RELEASE:=1

PKG_SOURCE_URL=https://github.com/blogic/ubtled.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2022-10-31
PKG_SOURCE_VERSION:=7e01ab86c562fc8ab3777d04e60b8dce596a4c5f

PKG_MAINTAINER:=John Crispin <john@phrozen.org>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/ubtled
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=OpenWrt BTLE daemon
  DEPENDS:=+libubox +libubus +bluez-libs
endef

define Package/ubtled/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/{config,init.d,uci-defaults}
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ubtled $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/ubtled.init $(1)/etc/init.d/ubtled
	$(INSTALL_DATA) ./files/btle.config $(1)/etc/config/btle
	$(INSTALL_DATA) ./files/99-btle $(1)/etc/uci-defaults/
endef

$(eval $(call BuildPackage,ubtled))

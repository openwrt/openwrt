include $(TOPDIR)/rules.mk

PKG_NAME:=ucentral-client
PKG_RELEASE:=1

#PKG_SOURCE_URL=https://github.com/blogic/ucentral-client.git
#PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-02-15
PKG_SOURCE_VERSION:=06704453bec36baf96919021507627b1f1d4e6e4

PKG_LICENSE:=BSD-3-Clause
PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/ucentral-client
  SECTION:=ucentral
  CATEGORY:=uCentral
  TITLE:=OpenWrt uCentral websocket client
  DEPENDS:=+ucode +ucode-mod-fs +ucode-mod-ubus +ucode-mod-uci +ucode-mod-math \
	   +libubox +libwebsockets-openssl +libblobmsg-json +libubus
endef

define Package/ucentral-client/conffiles
/etc/ucentral/
/etc/config-shadow/
endef

define Package/ucentral-client/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ucentral $(1)/usr/sbin/
	$(CP) ./files/* $(1)
endef

$(eval $(call BuildPackage,ucentral-client))

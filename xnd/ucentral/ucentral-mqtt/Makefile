include $(TOPDIR)/rules.mk

PKG_NAME:=ucentral-mqtt
PKG_RELEASE:=1

PKG_SOURCE_URL=https://github.com/blogic/ucentral-mqtt.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2022-11-27
PKG_SOURCE_VERSION:=6323938ad7f6b1b9dc172cdf6831dd7085c30100

PKG_MAINTAINER:=John Crispin <john@phrozen.org>
PKG_LICENSE:=BSD-3-Clause

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/ucentral-mqtt
  SECTION:=ucentral
  CATEGORY:=uCentral
  TITLE:=OpenWrt uCentral mqtt client
  DEPENDS:=+libubox +libubus +libblobmsg-json +libmosquitto-ssl
endef

define Package/ucentral-mqtt/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ucentral-mqtt $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/ucentral-mqtt $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/umqtt.config $(1)/etc/config/umqtt
	$(INSTALL_BIN) ./files/ucentral_stats.sh $(1)/usr/sbin
endef

$(eval $(call BuildPackage,ucentral-mqtt))

include $(TOPDIR)/rules.mk

PKG_NAME:=ipv6helper
PKG_RELEASE:=5

PKG_MAINTAINER:=Lean

include $(INCLUDE_DIR)/package.mk

define Package/ipv6helper
  SECTION:=ipv6
  TITLE:=IPv6 Helper and Dynamic Update he.net of ip
  DEPENDS:=+odhcpd-ipv6only +odhcp6c +6in4
  PKGARCH:=all
endef

define Package/ipv6helper/description
  IPv6 Helper and Dynamic Update he.net of ip
endef

define Build/Compile
endef

define Package/ipv6helper/install
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_BIN) ./files/60-6in4 $(1)/etc/hotplug.d/iface/
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/60-ipv6-lifetime $(1)/etc/uci-defaults/
endef

$(eval $(call BuildPackage,ipv6helper))

#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-dnsmasq-ipset
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/luci-app-dnsmasq-ipset
  SECTION:=luci
  CATEGORY:=LuCI
  DEPENDS:=+dnsmasq-full
  SUBMENU:=3. Applications
  TITLE:=support for IP Set settings
  PKGARCH:=all
endef

define Package/luci-app-dnsmasq-ipset/description
  Helper scripts for IP Set settings
endef


define Build/Compile
endef

define Package/luci-app-dnsmasq-ipset/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,luci-app-dnsmasq-ipset))

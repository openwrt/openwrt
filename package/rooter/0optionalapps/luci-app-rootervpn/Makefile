#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-rootervpn
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/luci-app-rootervpn
  SECTION:=luci
  CATEGORY:=LuCI
  DEPENDS:=+openvpn-easy-rsa +openvpn-openssl +eoip
  SUBMENU:=3. Applications
  TITLE:=support for modified OpenVPN
  PKGARCH:=all
endef

define Package/luci-app-rootervpn/description
  Helper scripts to enable OpenVPN support
endef


define Build/Compile
endef

define Package/luci-app-rootervpn/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,luci-app-rootervpn))

#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=vpnpolicy
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/vpnpolicy
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  DEPENDS:=+vpn-policy-routing +luci-app-vpn-policy-routing \
	+ipset +resolveip +ip +kmod-ipt-ipset +iptables +dnsmasq-full
  TITLE:=Install scripts for VPN Policy
  PKGARCH:=all
endef

define Package/vpnpolicy/description
  Helper scripts to install scripts for VPN Policy
endef


define Build/Compile
endef

$(eval $(call BuildPackage,vpnpolicy))

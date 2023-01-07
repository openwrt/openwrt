#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-wireguard
PKG_VERSION:=4.500
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-wireguard
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Optional Applications
  DEPENDS:=+wireguard-tools +kmod-wireguard \
		+luci-proto-wireguard +udp-tunnel +eoip
  TITLE:=Install scripts for Wireguard
  PKGARCH:=all
endef

define Package/ext-wireguard/description
  Install scripts for Wireguard
endef


define Build/Compile
endef

define Package/ext-wireguard/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ext-wireguard))

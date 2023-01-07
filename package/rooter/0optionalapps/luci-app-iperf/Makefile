#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-iperf
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/luci-app-iperf
  SECTION:=luci
  CATEGORY:=LuCI
  DEPENDS:=+iperf3-ssl
  SUBMENU:=3. Applications
  TITLE:=support for Iperf
  PKGARCH:=all
endef

define Package/luci-app-iperf/description
  Helper scripts to enable Iperf
endef


define Build/Compile
endef

define Package/luci-app-iperf/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,luci-app-iperf))

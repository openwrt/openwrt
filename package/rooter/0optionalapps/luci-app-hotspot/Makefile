#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-hotspot
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/luci-app-hotspot
  SECTION:=luci
  CATEGORY:=LuCI
  DEPENDS:=+iw +iwinfo
  SUBMENU:=3. Applications
  TITLE:=support for Wifi Hotspot Manager
  PKGARCH:=all
endef

define Package/luci-app-hotspot/description
  Helper scripts to enable Wifi Hotspot Manager
endef


define Build/Compile
endef

define Package/luci-app-hotspot/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,luci-app-hotspot))

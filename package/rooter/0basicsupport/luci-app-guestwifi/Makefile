#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-guestwifi
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/luci-app-guestwifi
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Basic Applications
  TITLE:=support for Guest Wifi
  PKGARCH:=all
endef

define Package/luci-app-guestwifi/description
  Helper scripts to enable Guest Wifi
endef


define Build/Compile
endef

define Package/luci-app-guestwifi/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,luci-app-guestwifi))

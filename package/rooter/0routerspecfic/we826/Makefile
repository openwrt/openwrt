#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=we826
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/we826
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  TITLE:=Install scripts for WE826 Watchdog
  PKGARCH:=all
endef

define Package/we826/description
  Helper scripts to install scripts for WE826 Watchdog
endef


define Build/Compile
endef

define Package/we826/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,we826))

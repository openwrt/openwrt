#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=d240
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/d240
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  TITLE:=Install scripts for D240
  PKGARCH:=all
endef

define Package/d240/description
  Helper scripts to install scripts for D240
endef


define Build/Compile
endef

define Package/d240/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,d240))

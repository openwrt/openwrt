#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=luci-theme-materialgrey
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/luci-theme-materialgrey
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Themes
  TITLE:=Install scripts for Material Grey theme
  PKGARCH:=all
endef

define Package/luci-theme-materialgrey/description
  Helper scripts to install scripts for Material Grey theme
endef


define Build/Compile
endef

define Package/luci-theme-materialgrey/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,luci-theme-materialgrey))

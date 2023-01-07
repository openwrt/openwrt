#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=luci-theme-argondark
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/luci-theme-argondark
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Themes
  TITLE:=Install scripts for Argondark theme
  PKGARCH:=all
endef

define Package/luci-theme-argondark/description
  Helper scripts to install scripts for Argondark theme
endef


define Build/Compile
endef

define Package/luci-theme-argondark/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,luci-theme-argondark))

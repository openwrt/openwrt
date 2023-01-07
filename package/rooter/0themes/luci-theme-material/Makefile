#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=luci-theme-material
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/luci-theme-material
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Themes
  TITLE:=Install scripts for Material theme login
  PKGARCH:=all
endef

define Package/luci-theme-material/description
  Helper scripts to install scripts for Material theme login
endef


define Build/Compile
endef

define Package/luci-theme-material/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,luci-theme-material))

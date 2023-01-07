#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-theme
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-theme
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Themes
  TITLE:=Install scripts for changing themes
  PKGARCH:=all
endef

define Package/ext-theme/description
  Helper scripts to install scripts for changing themes
endef


define Build/Compile
endef

define Package/ext-theme/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,ext-theme))

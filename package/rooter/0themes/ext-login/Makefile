#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-login
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-login
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Themes
  TITLE:=Install scripts for Login Backgrounds
  PKGARCH:=all
endef

define Package/ext-login/description
  Helper scripts to install scripts for Login Backgrounds
endef


define Build/Compile
endef

define Package/ext-login/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,ext-login))

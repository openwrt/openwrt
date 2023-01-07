#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-splashconfig
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-splashconfig
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Splash Screens
  DEPENDS:=+ext-splash
  TITLE:=Added scripts for Login display
  PKGARCH:=all
endef

define Package/ext-splashconfig/description
  Added scripts for Login display
endef


define Build/Compile
endef

define Package/ext-splashconfig/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,ext-splashconfig))

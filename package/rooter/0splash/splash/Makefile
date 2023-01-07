#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=splash
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/splash
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Splash Screens
  TITLE:=Added scripts for Splash screen
  PKGARCH:=all
endef

define Package/splash/description
  Added scripts for Splash screen
endef


define Build/Compile
endef

define Package/splash/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,splash))

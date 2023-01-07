#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=status
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/status
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Splash Screens
  TITLE:=Added scripts for Status Splash screen
  PKGARCH:=all
endef

define Package/status/description
  Added scripts for Status Splash screen
endef


define Build/Compile
endef

define Package/status/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,status))

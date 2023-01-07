#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=extramenu
PKG_VERSION:=4.500
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/extramenu
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Optional Applications
  TITLE:=Install scripts for Extra menus
  PKGARCH:=all
endef

define Package/extramenu/description
  Install scripts for Extra menus
endef


define Build/Compile
endef

define Package/extramenu/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,extramenu))

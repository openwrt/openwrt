#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-menu
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-menu
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Optional Applications
  TITLE:=Menu Selection
  PKGARCH:=all
endef

define Package/ext-menu/description
  Helper scripts to install Menu Selection on ROOter
endef


define Build/Compile
endef

define Package/ext-menu/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,ext-menu))

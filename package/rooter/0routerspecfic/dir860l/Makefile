#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=dir860l
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/dir860l
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  TITLE:=Install scripts for Dlink DIR-860L
  PKGARCH:=all
endef

define Package/dir860l/description
  Helper scripts to install scripts for Dlink DIR-860L
endef


define Build/Compile
endef

define Package/dir860l/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,dir860l))

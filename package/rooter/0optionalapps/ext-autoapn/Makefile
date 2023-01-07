#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-autoapn
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-autoapn
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Optional Applications
  TITLE:=support for Automatic APN
  PKGARCH:=all
endef

define Package/ext-autoapn/description
  Helper scripts to enable Automatic APN
endef


define Build/Compile
endef

define Package/ext-autoapn/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ext-autoapn))

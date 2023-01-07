#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-netselect
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-netselect
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Optional Applications
  TITLE:=support for Network Selection
  PKGARCH:=all
endef

define Package/ext-netselect/description
  Helper scripts to enable Network Selection
endef


define Build/Compile
endef

define Package/ext-netselect/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ext-netselect))

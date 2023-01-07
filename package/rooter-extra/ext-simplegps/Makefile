#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-simplegps
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-simplegps
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Optional Applications
  DEPENDS:=+msmtp
  TITLE:=support for gps
  PKGARCH:=all
endef

define Package/ext-simplegps/description
  Helper scripts to enable gps
endef


define Build/Compile
endef

define Package/ext-simplegps/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ext-simplegps))

#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=pingtest
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/pingtest
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Optional Applications
  DEPENDS:=+httping +curl
  TITLE:=Added scripts for custom ping test
  PKGARCH:=all
endef

define Package/pingtest/description
  Added scripts for custom ping test
endef


define Build/Compile
endef

define Package/pingtest/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,pingtest))

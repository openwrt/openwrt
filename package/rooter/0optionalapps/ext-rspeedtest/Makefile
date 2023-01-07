#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-rspeedtest
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-rspeedtest
  SECTION:=utils
  CATEGORY:=ROOter
  DEPENDS:=+httping +curl +coreutils +coreutils-date +speedtestpp
  SUBMENU:=Optional Applications
  TITLE:=support for ROOter SpeedTest
  PKGARCH:=all
endef

define Package/ext-rspeedtest/description
  Helper scripts to enable ROOter SpeedTest
endef


define Build/Compile
endef

define Package/ext-rspeedtest/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ext-rspeedtest))

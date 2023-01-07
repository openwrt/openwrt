#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-speedtest
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-speedtest
  SECTION:=utils
  CATEGORY:=ROOter
  DEPENDS:=+ext-rspeedtest
  SUBMENU:=Optional Applications
  TITLE:=support for OpenSpeedTest
  PKGARCH:=all
endef

define Package/ext-speedtest/description
  Helper scripts to enable OpenSpeedTest
endef


define Build/Compile
endef

define Package/ext-speedtest/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ext-speedtest))

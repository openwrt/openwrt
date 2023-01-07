#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=wrt1900
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/wrt1900
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  TITLE:=Install scripts for WRT1900
  PKGARCH:=all
endef

define Package/wrt1900/description
  Helper scripts to install scripts for WRT1900
endef


define Build/Compile
endef

define Package/wrt1900/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,wrt1900))

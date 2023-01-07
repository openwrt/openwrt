#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-blockport
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-blockport
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Optional Applications
  TITLE:=Install Port Blocking
  PKGARCH:=all
endef

define Package/ext-blockport/description
  Helper scripts to install Port Blocking
endef


define Build/Compile
endef

define Package/ext-blockport/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,ext-blockport))

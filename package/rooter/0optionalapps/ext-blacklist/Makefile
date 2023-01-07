#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-blacklist
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-blacklist
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Optional Applications
  TITLE:=Install Mac Blacklisting
  PKGARCH:=all
endef

define Package/ext-blacklistdescription
  Helper scripts to install Mac Blacklisting
endef


define Build/Compile
endef

define Package/ext-blacklist/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,ext-blacklist))

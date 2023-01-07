#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=wg209
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/wg209
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  TITLE:=Install scripts for WG209
  PKGARCH:=all
endef

define Package/wg209/description
  Helper scripts to install scripts for WG209
endef


define Build/Compile
endef

define Package/wg209/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,wg209))

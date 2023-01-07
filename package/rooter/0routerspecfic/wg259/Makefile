#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=wg259
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/wg259
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  DEPENDS:=+kmod-mt76 +kmod-mt7603 +kmod-mt76x2
  TITLE:=Install scripts for WG259
  PKGARCH:=all
endef

define Package/wg259/description
  Helper scripts to install scripts for WG259
endef


define Build/Compile
endef

define Package/wg259/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,wg259))

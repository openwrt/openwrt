#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=alix2d13
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/alix2d13
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  TITLE:=Install scripts for alix2d13
  PKGARCH:=all
endef

define Package/alix2d13/description
  Helper scripts to install scripts for alix2d13
endef


define Build/Compile
endef

define Package/alix2d13/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,alix2d13))

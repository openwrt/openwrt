#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=x750
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/x750
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  TITLE:=Install scripts for x750
  PKGARCH:=all
endef

define Package/x750/description
  Helper scripts to install scripts for x750
endef


define Build/Compile
endef

define Package/x750/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,x750))

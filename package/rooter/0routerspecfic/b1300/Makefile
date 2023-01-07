#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=b1300
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/b1300
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  TITLE:=Install scripts for B1300
  PKGARCH:=all
endef

define Package/b1300/description
  Helper scripts to install scripts for B1300
endef


define Build/Compile
endef

define Package/b1300/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,b1300))

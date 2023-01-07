#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=mt1300
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/mt1300
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  TITLE:=Install scripts for MT1300
  PKGARCH:=all
endef

define Package/mt1300/description
  Helper scripts to install scripts for MT1300
endef


define Build/Compile
endef

define Package/mt1300/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,mt1300))

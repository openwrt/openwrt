#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=rbsxtr
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/rbsxtr
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  TITLE:=Install scripts for RBSXTR
  PKGARCH:=all
endef

define Package/rbsxtr/description
  Helper scripts to install scripts for RBSXTR
endef


define Build/Compile
endef

define Package/rbsxtr/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,rbsxtr))

#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=bwmon
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/bwmon
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Optional Applications
  TITLE:=Install Bandwidth Monitor
  PKGARCH:=all
endef

define Package/bwmon/description
  Helper scripts to install Bandwidth Monitor on ROOter
endef


define Build/Compile
endef

define Package/bwmon/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,bwmon))

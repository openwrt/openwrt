#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=webconsole
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/webconsole
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Optional Applications
  TITLE:=Install scripts for Web Console
  PKGARCH:=all
endef

define Package/webconsole/description
  Helper scripts to install scripts for Web Console
endef


define Build/Compile
endef

define Package/webconsole/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,webconsole))

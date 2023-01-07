#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-texting
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-texting
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Optional Applications
  TITLE:=support for Random Texting
  PKGARCH:=all
endef

define Package/ext-texting/description
  Helper scripts to enable Random Texting
endef


define Build/Compile
endef

define Package/ext-texting/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ext-texting))

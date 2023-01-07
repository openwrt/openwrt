#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=wg3526
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/wg3526
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  TITLE:=Install scripts for wg3526
  PKGARCH:=all
endef

define Package/wg3526/description
  Helper scripts to install scripts for wg3526
endef


define Build/Compile
endef

define Package/wg3526/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,wg3526))

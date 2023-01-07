#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=fscheck
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/fscheck
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  DEPENDS:=+e2fsprogs
  TITLE:=Install scripts for FSCheck
  PKGARCH:=all
endef

define Package/fscheck/description
  Helper scripts to install scripts for FSCheck
endef


define Build/Compile
endef

define Package/fscheck/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,fscheck))

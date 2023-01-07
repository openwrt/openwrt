#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=bwallocate
PKG_VERSION:=4.500
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/bwallocate
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Optional Applications
  DEPENDS:=+ext-throttle +msmtp
  TITLE:=Install scripts for Bandwidth Allocation
  PKGARCH:=all
endef

define Package/bwallocate/description
  Install scripts for Bandwidth Allocation
endef


define Build/Compile
endef

define Package/bwallocate/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,bwallocate))

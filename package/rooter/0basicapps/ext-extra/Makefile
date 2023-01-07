#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-extra
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-extra
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Basic Applications
  TITLE:=Add Scheduled Reboot
  PKGARCH:=all
endef

define Package/ext-extra/description
  Helper scripts to install Scheduled Reboot
endef


define Build/Compile
endef

define Package/ext-extra/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,ext-extra))

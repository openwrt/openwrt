#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-command
PKG_VERSION:=4.500
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-command
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Basic Applications
  TITLE:=support for Command
  PKGARCH:=all
endef

define Package/ext-command/description
  Helper scripts to enable command
endef


define Build/Compile
endef

define Package/ext-command/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ext-command))

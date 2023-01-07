#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-sms
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-sms
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Basic Support
  TITLE:=Install SMS support
  PKGARCH:=all
endef

define Package/ext-sms/description
  Helper scripts to install SMS on ROOter
endef


define Build/Compile
endef

define Package/ext-sms/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,ext-sms))

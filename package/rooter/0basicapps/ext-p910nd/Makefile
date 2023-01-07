#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-p910nd
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-p910nd
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Basic Applications
  DEPENDS:=+p910nd +kmod-usb-printer +kmod-usb-ohci
  TITLE:=Install Print Server
  PKGARCH:=all
endef

define Package/ext-p910nd/description
  Helper scripts to install print Server
endef


define Build/Compile
endef

define Package/ext-p910nd/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,ext-p910nd))

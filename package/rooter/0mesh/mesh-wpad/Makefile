#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=mesh-wpad
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/mesh-wpad
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Mesh Support
  DEPENDS:=+wpad-mesh-openssl
  TITLE:=Support for Mesh routing
  PKGARCH:=all
endef

define Package/mesh-wpad/description
  Helper scripts to install Meshing
endef

define Build/Compile
endef

$(eval $(call BuildPackage,mesh-wpad))

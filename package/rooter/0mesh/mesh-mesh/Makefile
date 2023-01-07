#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=mesh-mesh
PKG_VERSION:=4.500
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/mesh-mesh
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Mesh Support
  DEPENDS:=+kmod-batman-adv +alfred +batctl +ip
  TITLE:=Install scripts for Mesh Network
  PKGARCH:=all
endef

define Package/mesh-mesh/description
  Install scripts for Mesh Network
endef


define Build/Compile
endef

define Package/mesh-mesh/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,mesh-mesh))

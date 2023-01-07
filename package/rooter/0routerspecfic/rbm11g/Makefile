#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=rbm11g
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/rbm11g
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  TITLE:=Install scripts for RBM11G
  PKGARCH:=all
endef

define Package/rbm11g/description
  Helper scripts to install scripts for RBM11G
endef


define Build/Compile
endef

define Package/rbm11g/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,rbm11g))

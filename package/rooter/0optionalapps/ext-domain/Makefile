#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-domain
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-domain
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Optional Applications
  DEPENDS:=+dnsmasq-full +ipset
  TITLE:=Domain Filter
  PKGARCH:=all
endef

define Package/ext-domain/description
  Helper scripts to install Domain Filter on ROOter
endef


define Build/Compile
endef

define Package/ext-domain/install
	$(CP) ./files/* $(1)/


endef

$(eval $(call BuildPackage,ext-domain))

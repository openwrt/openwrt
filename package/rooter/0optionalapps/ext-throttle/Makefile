#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-throttle
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-throttle
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Optional Applications
  DEPENDS:=+tc +kmod-sched-core +kmod-ifb +iptables \
	+iptables-mod-ipopt +iptables-mod-conntrack-extra \
	+kmod-sched-cake
  TITLE:=support for Throttle
  PKGARCH:=all
endef

define Package/ext-throttle/description
  Helper scripts to enable Throttle
endef


define Build/Compile
endef

define Package/ext-throttle/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ext-throttle))

#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=ext-rooter4
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/ext-rooter4
  SECTION:=utils
  CATEGORY:=ROOter
  DEPENDS:=+luci +ext-rooter-basic \
		+ca-bundle +ca-certificates \
		+luci-app-sqm +kmod-sched-cake \
		+luci-app-ddns +ddns-scripts \
		+ext-command +ext-p910nd +luci-app-guestwifi \
		+luci-app-hotspot +ext-extra \
		+picocom +wget +nano +464xlat 
  TITLE:=install basic ROOter routers
  PKGARCH:=all
endef

define Package/ext-rooter4/description
  Helper scripts to install basic ROOter routers
endef

define Build/Compile
endef

define Package/ext-rooter4/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,ext-rooter4))

#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=wg827
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/wg827
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  DEPENDS:=+kmod-mt7615e +kmod-mt76 +kmod-mt7603 +kmod-mt7615-firmware \
		+kmod-mt7615e +kmod-mt7663-firmware-ap +kmod-mt7663-firmware-sta \
		+kmod-mt76x2
  TITLE:=Install scripts for WG827
  PKGARCH:=all
endef

define Package/wg827/description
  Helper scripts to install scripts for WG827
endef


define Build/Compile
endef

define Package/wg827/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,wg827))

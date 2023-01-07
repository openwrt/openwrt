#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=h721
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/h721
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  DEPENDS:=+kmod-ath10k +kmod-ath9k \
	 +ath10k-firmware-qca988x +ath10k-firmware-qca9984 \
	 +ath10k-firmware-qca99x0 +ath10k-firmware-qca9888
  TITLE:=Install scripts for Dual-Q H721
  PKGARCH:=all
endef

define Package/h721/description
  Helper scripts to install scripts for Dual-Q H721
endef


define Build/Compile
endef

define Package/h721/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,h721))

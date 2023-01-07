#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=apu2c4
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

define Package/apu2c4
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Router Specific
  DEPENDS:=+kmod-leds-gpio +kmod-crypto-hw-ccp \
	+kmod-gpio-nct5104d +kmod-gpio-button-hotplug \
	+kmod-usb-core +kmod-usb-ohci +kmod-usb2 +kmod-usb3 \
	+kmod-sound-core +kmod-pcspkr
  TITLE:=Install scripts for apu2c4
  PKGARCH:=all
endef

define Package/apu2c4/description
  Helper scripts to install scripts for apu2c4
endef


define Build/Compile
endef

$(eval $(call BuildPackage,apu2c4))

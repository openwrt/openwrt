#Owned by DairyMan@Whirlpool
#
#Copyright GNU act.
include $(TOPDIR)/rules.mk

PKG_NAME:=extra-wifi
PKG_VERSION:=1.000
PKG_RELEASE:=1

PKG_MAINTAINER:=Created by DM/makefile by Cobia@whirlpool
include $(INCLUDE_DIR)/package.mk

#+kmod-ath9k +kmod-rt2500-usb +kmod-rt73-usb +kmod-rtl8187 \
#	   +kmod-rtl8192cu +kmod-rtl8xxxu +kmod-net-rtl8192su \
#	   +kmod-usb-net-rtl8152 +kmod-usb-net-asix-ax88179 \
#	   +kmod-usb-net-asix +kmod-mt76 +kmod-mt7601u

define Package/extra-wifi
  SECTION:=utils
  CATEGORY:=ROOter
  SUBMENU:=Wifi
  DEPENDS:=+kmod-ath9k +kmod-rt2500-usb +kmod-rt73-usb +kmod-rtl8187 \
		 +kmod-rtl8192cu +kmod-rtl8xxxu +kmod-net-rtl8192su \
		+kmod-mt76 +kmod-mt7601u
  TITLE:=support for wifi
  PKGARCH:=all
endef

define Package/extra-wifi/description
  Helper scripts to enable wifi
endef


define Build/Compile
endef


$(eval $(call BuildPackage,extra-wifi))

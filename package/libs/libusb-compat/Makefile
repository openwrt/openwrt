#
# Copyright (C) 2010-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libusb-compat
PKG_VERSION:=0.1.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/libusb
PKG_MD5SUM:=2ca521fffadd0c28fdf174e6ec73865b

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

PKG_MAINTAINER := Felix Fietkau <nbd@openwrt.org>

include $(INCLUDE_DIR)/package.mk

define Package/libusb-compat
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=libusb-0.1 compatibility library
  DEPENDS:=+libusb-1.0
  URL:=http://libusb.wiki.sourceforge.net/
endef

define Package/libusb-compat/description
  libusb is a C library that gives applications easy access to USB devices on
  many different operating systems.
endef

TARGET_CFLAGS += $(FPIC)

define Build/InstallDev
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/libusb-config $(STAGING_DIR)/host/bin
endef

define Package/libusb-compat/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libusb-0.1.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libusb-compat))

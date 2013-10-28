#
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=cyassl
PKG_VERSION:=2.8.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).zip
PKG_SOURCE_URL:=http://www.yassl.com/
PKG_MD5SUM:=7465d4815af90eff01095fa1b031ce09

PKG_FIXUP:=patch-libtool
PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libcyassl
  SECTION:=libs
  SUBMENU:=SSL
  CATEGORY:=Libraries
  TITLE:=CyaSSL library
  URL:=http://www.yassl.com/
endef

define Package/libcyassl/description
CyaSSL is an SSL library optimized for small footprint, both on disk and for
memory use.
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--without-zlib \
	--enable-singlethreaded \
	--disable-examples

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcyassl.{so*,la} $(1)/usr/lib/
endef

define Package/libcyassl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcyassl.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libcyassl))

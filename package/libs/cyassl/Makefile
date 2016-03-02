#
# Copyright (C) 2006-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wolfssl
PKG_VERSION:=3.8.0
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).zip
PKG_SOURCE_URL:=https://www.wolfssl.com/
PKG_MD5SUM:=a73d90c5439adea97a5002a73b46ddeb

PKG_FIXUP:=libtool
PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1
PKG_LICENSE:=GPL-2.0+

include $(INCLUDE_DIR)/package.mk

define Package/libcyassl
  SECTION:=libs
  SUBMENU:=SSL
  CATEGORY:=Libraries
  TITLE:=CyaSSL library
  URL:=http://www.wolfssl.com/
endef

define Package/libcyassl/description
CyaSSL is an SSL library optimized for small footprint, both on disk and for
memory use.
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--without-zlib \
	--enable-singlethreaded \
	--enable-opensslextra \
	--enable-sni \
	--enable-ecc \
	--enable-sslv3 \
	--disable-examples

#ifneq ($(CONFIG_TARGET_x86),)
#	CONFIGURE_ARGS += --enable-intelasm
#endif
#ifneq ($(CONFIG_TARGET_x86_64),)
#	CONFIGURE_ARGS += --enable-intelasm
#endif

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libwolfssl.{so*,la} $(1)/usr/lib/
	ln -s libwolfssl.so $(1)/usr/lib/libcyassl.so
	ln -s libwolfssl.la $(1)/usr/lib/libcyassl.la
endef

define Package/libcyassl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libwolfssl.so* $(1)/usr/lib/
	ln -s libwolfssl.so $(1)/usr/lib/libcyassl.so
endef

$(eval $(call BuildPackage,libcyassl))

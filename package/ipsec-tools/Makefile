# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ipsec-tools
PKG_VERSION:=0.6.6
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/ipsec-tools
PKG_MD5SUM:=e908f3cf367e31c7902df5ab16fbe5c3
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/ipsec-tools
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=@LINUX_2_6 +libopenssl
  TITLE:=IPsec management tools
  URL:=http://ipsec-tools.sourceforge.net/
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); touch \
		configure.ac \
		aclocal.m4 \
		Makefile.in \
		config.h.in \
		configure \
	);
	$(call Build/Configure/Default, \
		--enable-shared \
		--enable-static \
		--with-kernel-headers="$(LINUX_DIR)/include" \
		--without-readline \
		--with-openssl="$(STAGING_DIR)/usr" \
		--without-libradius \
		--without-libpam \
		--enable-dpd \
		--enable-natt \
	);
endef

# override CFLAGS holding "-Werror" that break builds on compile warnings
define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		all install
endef

define Package/ipsec-tools/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/src/racoon/samples/racoon.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libipsec.so.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libracoon.so.* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/plainrsa-gen $(1)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/racoon $(1)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/racoonctl $(1)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/setkey $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ipsec-tools))

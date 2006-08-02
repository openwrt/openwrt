# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=hostap-utils
PKG_VERSION:=0.4.7
PKG_RELEASE:=1
PKG_MD5SUM:=afe041581b8f01666e353bec20917c85

PKG_SOURCE_URL:=http://hostap.epitest.fi/releases/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/hostap-utils
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+hostap
  TITLE:=Host AP driver utility programs
  DESCRIPTION:=Host AP driver utility programs
  URL:=http://hostap.epitest.fi/
endef


define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include -Wall" \
		all
endef

define Package/hostap-utils/install	
	install -m0755 -d $(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/hostap_crypt_conf $(1)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/hostap_diag $(1)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/hostap_io_debug $(1)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/hostap_rid $(1)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/prism2_srec $(1)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/split_combined_hex $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,hostap-utils))

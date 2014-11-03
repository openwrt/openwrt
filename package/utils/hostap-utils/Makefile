# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=hostap-utils
PKG_VERSION:=0.4.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://hostap.epitest.fi/releases/
PKG_MD5SUM:=afe041581b8f01666e353bec20917c85

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=COPYING

include $(INCLUDE_DIR)/package.mk

define Package/hostap-utils
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=kmod-hostap
  TITLE:=Host AP driver utility programs
  URL:=http://hostap.epitest.fi/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS) -Wall" \
		all
endef

define Package/hostap-utils/install	
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hostap_crypt_conf $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hostap_diag $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hostap_io_debug $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hostap_rid $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/prism2_srec $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/split_combined_hex $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,hostap-utils))

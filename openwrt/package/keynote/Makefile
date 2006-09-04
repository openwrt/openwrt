# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 4619 2006-08-22 09:50:02Z florian $

include $(TOPDIR)/rules.mk

PKG_NAME:=keynote
PKG_VERSION:=2.3
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources/
PKG_MD5SUM:=b569066ac2ba1356c2112b118a7d74d0
PKG_CAT:=bzcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/keynote
SECTION:=base
CATEGORY:=Network
DEPENDS:=@LINUX_2_6
TITLE:=IPsec management tools
DESCRIPTION:=IPsec management tools
URL:=http://www1.cs.columbia.edu/~angelos/keynote.html
endef

define Build/Configure
$(call Build/Configure/Default,--enable-static --enable-shared,LDFLAGS="-L$(STAGING_DIR)/usr/lib")
endef

define Build/Compile
	$(call Build/Compile/Default,$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		all)
endef

define Package/keynote/install
	install -d -m0755 $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/keynote $(1)/usr/sbin/
endef

define Build/InstallDev
	$(CP) $(PKG_BUILD_DIR)/*.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/libkeynote.a $(STAGING_DIR)/usr/lib/
endef

define Build/UninstallDev
	rm -rf $(STAGING_DIR)/usr/include/{assertion,getopt,header,keynote,signature}.h
endef

$(eval $(call BuildPackage,keynote))

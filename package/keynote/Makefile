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

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/keynote
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=@LINUX_2_6 +libopenssl
  TITLE:=Simple and flexible trust-management system
  DESCRIPTION:=\
	KeyNote is a simple and flexible trust-management system designed to work \\\
	well for a variety of large- and small- scale Internet-based applications.\\\
	It provides a single, unified language for both local policies and \\\
	credentials. 
  URL:=http://www1.cs.columbia.edu/~angelos/keynote.html
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--enable-static \
		--enable-shared \
	)
endef

define Build/Compile
	$(call Build/Compile/Default, \
		CFLAGS="\$$$$(EXTRA_CFLAGS) \$$$$(EXTRA_LDFLAGS)" \
	)
endef

define Build/InstallDev
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_BUILD_DIR)/{assertion,header,keynote,signature}.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libkeynote.a $(STAGING_DIR)/usr/lib/
endef

define Build/UninstallDev
	rm -rf \
		$(STAGING_DIR)/usr/include/keynote \
		$(STAGING_DIR)/usr/lib/libkeynote.a
endef

define Package/keynote/install
	install -d -m0755 $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/keynote $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,keynote))

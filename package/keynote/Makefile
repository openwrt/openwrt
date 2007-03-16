# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=keynote
PKG_VERSION:=2.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources/
PKG_MD5SUM:=b569066ac2ba1356c2112b118a7d74d0
PKG_CAT:=bzcat

include $(INCLUDE_DIR)/package.mk

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

CONFIGURE_ARGS += \
	--enable-static \
	--enable-shared

define Build/Compile
	$(call Build/Compile/Default, \
		LIBS="$(EXTRA_LDFLAGS) -L. -lkeynote -lm -lcrypto" \
	)
endef

define Build/InstallDev
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_BUILD_DIR)/{assertion,header,keynote,signature}.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libkeynote.a $(STAGING_DIR)/usr/lib/
endef

define Build/UninstallDev
	rm -rf	$(STAGING_DIR)/usr/include/keynote \
		$(STAGING_DIR)/usr/lib/libkeynote.a
endef

define Package/keynote/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/keynote $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,keynote))

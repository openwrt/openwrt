# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$
#
include $(TOPDIR)/rules.mk

PKG_NAME:=ipset
PKG_VERSION:=2.2.9a
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-20061009.tar.bz2
PKG_SOURCE_URL:=http://ipset.netfilter.org
PKG_MD5SUM:=74ce3226b30cdd386630250bdc3488ef

include $(INCLUDE_DIR)/package.mk

define Package/ipset
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Netfilter ip sets administration utility
  URL:=http://ipset.netfilter.org/
endef

define Build/Compile
	$(call Build/Compile/Default, \
		COPT_FLAGS="$(TARGET_CFLAGS)" \
		KERNEL_DIR="$(LINUX_DIR)" \
		PREFIX="/usr" \
	)
endef

define Package/ipset/install
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(1)" \
		PREFIX="/usr" \
		install
	rm -rf $(1)/usr/man
endef

$(eval $(call BuildPackage,ipset))

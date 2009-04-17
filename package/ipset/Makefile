# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
#
include $(TOPDIR)/rules.mk

PKG_NAME:=ipset
PKG_VERSION:=2.3.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-20070828.tar.bz2
PKG_SOURCE_URL:=http://ipset.netfilter.org
PKG_MD5SUM:=9e17798dfd8ed87c63a1f3498f9fe64d

include $(INCLUDE_DIR)/package.mk

define Package/ipset
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=@!TARGET_etrax @LINUX_2_6
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

# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ipset
PKG_VERSION:=3.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://ipset.netfilter.org
PKG_MD5SUM:=0fd83af3efae5f72f5e5b55d07582941

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
		LDFLAGS="-rdynamic -static-libgcc" \
		KERNEL_DIR="$(LINUX_DIR)" \
		PREFIX="/usr" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		binaries \
	)
endef

define Package/ipset/install
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(1)" \
		PREFIX="/usr" \
		binaries_install
	rm -rf $(1)/usr/man
endef

$(eval $(call BuildPackage,ipset))

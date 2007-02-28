# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=kexec-tools
PKG_VERSION:=1.101
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.xmission.com/~ebiederm/files/kexec/
PKG_MD5SUM:=b4f7ffcc294d41a6a4c40d6e44b7734d
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/kexec-tools
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@LINUX_2_6_X86 +zlib
  TITLE:=Kernel boots kernel
  DESCRIPTION:=\
	kexec is a set of systems call that allows you to load \\\
	another kernel from the currently executing Linux kernel.
  URL:=http://www.xmission.com/~ebiederm/files/kexec/README
endef

MAKE_FLAGS += \
	DESTDIR="$(PKG_INSTALL_DIR)" \
	EXTRA_CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
	BUILD_CFLAGS="-O2" \
	LIBS="$(TARGET_LDFLAGS) -lz" \
	install

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(call Build/Compile/Default,)
endef

define Package/kexec-tools/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/kexec $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/kdump $(1)/usr/bin/
endef

$(eval $(call BuildPackage,kexec-tools))

# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=kexec-tools
PKG_VERSION:=testing-20080324
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://kernel.org/pub/linux/kernel/people/horms/kexec-tools/
PKG_MD5SUM:=9f442520b0e238ae5beb4ebea0867377

include $(INCLUDE_DIR)/package.mk

define Package/kexec-tools
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@i386||@TARGET_ps3||@mipsel +zlib
  TITLE:=Kernel boots kernel
  URL:=http://kernel.org/pub/linux/kernel/people/horms/kexec-tools/
  MENU:=1
endef

define Package/kexec-tools/description
 kexec is a set of systems call that allows you to load
 another kernel from the currently executing Linux kernel.
endef

define Package/kexec-tools/config
	source "$(SOURCE)/kexec-config.in"
endef

CONFIGURE_ARGS = \
		--target=$(CONFIG_KEXEC_TOOLS_TARGET_NAME)-linux-uclibc \
		--host=$(GNU_TARGET_NAME)-uclibc \
		--build=$(GNU_HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \

CONFIGURE_VARS += BUILD_CC=$(HOSTCC)

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) all
endef

define Package/kexec-tools/install
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(1)" \
		install
	rm -rf $(1)/usr/man
endef

$(eval $(call BuildPackage,kexec-tools))

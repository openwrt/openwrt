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
  DEPENDS:=@LINUX_2_6_X86
  TITLE:=Kernel boots kernel
  DESCRIPTION:=\
	kexec is a set of systems call that allows you to load \\\
	another kernel from the currently executing Linux kernel.
  URL:=http://www.xmission.com/~ebiederm/files/kexec/README
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
		  --target=$(GNU_TARGET_NAME) \
		  --host=$(GNU_TARGET_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --program-prefix="" \
		  --program-suffix="" \
		  --prefix=/usr \
		  --exec-prefix=/usr \
		  --bindir=/usr/bin \
		  --sbindir=/usr/sbin \
		  --libexecdir=/usr/lib \
		  --sysconfdir=/etc \
		  --datadir=/usr/share \
		  --localstatedir=/var \
		  --mandir=/usr/man \
		  --infodir=/usr/info \
		  $(DISABLE_NLS) \
		  $(DISABLE_LARGEFILE) \
	);
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
endef

define Package/kexec-tools/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/kexec $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/kdump $(1)/usr/bin/
endef

$(eval $(call BuildPackage,kexec-tools))

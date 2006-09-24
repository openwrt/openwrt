# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=fuse
PKG_VERSION:=2.5.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=9c7e8b6606b9f158ae20b8521ba2867c
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/fuse/Default
  TITLE:=FUSE
  DESCRIPTION:=\
	FUSE (Filesystem in UserSpacE)
  URL:=http://fuse.sourceforge.net/
endef

define Package/fuse-utils
  $(call Package/fuse/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE+= (utilities)
  DESCRIPTION+=\\\
	\\\
	This package contains the FUSE utilities.
endef

define Package/kmod-fuse
  $(call Package/fuse/Default)
  SECTION:=kernel
  CATEGORY:=Kernel drivers
  TITLE+= (kernel module)
  DESCRIPTION+=\\\
	\\\
	This package contains the FUSE kernel module.
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Package/libfuse
  $(call Package/fuse/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= (library)
  DESCRIPTION+=\\\
	\\\
	This package contains the FUSE shared library, needed by other programs.
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); rm -f config.cache; \
		touch configure.in ; \
		touch aclocal.m4 ; \
		touch Makefile.in ; \
		touch include/config.h.in ; \
		touch configure ; \
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
			--datadir=/usr/share \
			--includedir=/usr/include \
			--infodir=/usr/share/info \
			--libdir=/usr/lib \
			--libexecdir=/usr/lib \
			--localstatedir=/var \
			--mandir=/usr/share/man \
			--sbindir=/usr/sbin \
			--sysconfdir=/etc \
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
			--enable-shared \
			--enable-static \
			--disable-rpath \
			--enable-kernel-module \
			--enable-lib \
			--enable-util \
			--disable-example \
			--disable-auto-modprobe \
			--with-kernel="$(LINUX_DIR)" \
			--disable-mtab \
	);
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Build/InstallDev
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP)	$(PKG_INSTALL_DIR)/usr/include/fuse{,.h} $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP)	$(PKG_INSTALL_DIR)/usr/lib/libfuse.{a,so*} $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/fuse.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	$(SED) 's,-I$$$${includedir}/fuse,,g' $(STAGING_DIR)/usr/lib/pkgconfig/fuse.pc
	$(SED) 's,-L$$$${libdir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/fuse.pc
endef
	
define Build/UninstallDev
	rm -rf	$(STAGING_DIR)/usr/include/fuse{,.h} \
		$(STAGING_DIR)/usr/lib/libfuse.{a,so*} \
		$(STAGING_DIR)/usr/lib/pkgconfig/fuse.pc
endef

define Package/fuse-utils/install
	install -d -m0755 $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/fusermount $(1)/usr/bin/
endef

define Package/kmod-fuse/install
	install -d -m0755 $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_INSTALL_DIR)/lib/modules/$(LINUX_VERSION)/kernel/fs/fuse/fuse.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
	install -d -m0755 $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfuse.so.* $(1)/usr/lib/
	install -d -m0755 $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/fusermount $(1)/usr/bin/
endef

define Package/libfuse/install
	install -d -m0755 $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfuse.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,fuse-utils))
$(eval $(call BuildPackage,kmod-fuse))
$(eval $(call BuildPackage,libfuse))

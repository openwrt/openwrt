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
PKG_VERSION:=2.6.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=3f4f5d07d12dedc0a3fd23472171fe94
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/fuse/Default
  TITLE:=FUSE
  DEPENDS:=@LINUX_2_6
  DESCRIPTION:=\
	FUSE (Filesystem in UserSpacE)
  URL:=http://fuse.sourceforge.net/
endef

define Package/fuse-utils
  $(call Package/fuse/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libfuse +kmod-fuse @LINUX_2_6
  TITLE+= (utilities)
  DESCRIPTION+=\\\
	\\\
	This package contains the FUSE utilities.
endef

define KernelPackage/fuse
  SUBMENU:=Filesystems
  $(call Package/fuse/Default)
  TITLE+= (kernel module)
  DEPENDS:=@LINUX_2_6
  DESCRIPTION+=\\\
	\\\
	This package contains the FUSE kernel module.
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
  FILES:=$(PKG_INSTALL_DIR)/lib/modules/$(LINUX_VERSION)/kernel/fs/fuse/fuse.$(LINUX_KMOD_SUFFIX)
endef

define Package/libfuse
  $(call Package/fuse/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=@LINUX_2_6
  TITLE+= (library)
  DESCRIPTION+=\\\
	\\\
	This package contains the FUSE shared library, needed by other programs.
endef

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-rpath \
	--enable-kernel-module \
	--enable-lib \
	--enable-util \
	--disable-example \
	--disable-auto-modprobe \
	--with-kernel="$(LINUX_DIR)" \
	--disable-mtab

define Build/Configure
	(cd $(PKG_BUILD_DIR); rm -f config.cache; \
		touch configure.in ; \
		touch aclocal.m4 ; \
		touch Makefile.in ; \
		touch include/config.h.in ; \
		touch configure ; \
	)
	$(call Build/Configure/Default)
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
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/fusermount $(1)/usr/bin/
endef

define Package/libfuse/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfuse.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,fuse-utils))
$(eval $(call BuildPackage,libfuse))
$(eval $(call KernelPackage,fuse))

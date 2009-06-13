# 
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=fuse

PKG_VERSION:=2.5.3
PKG_RELEASE:=1
PKG_MD5SUM:=9c7e8b6606b9f158ae20b8521ba2867c

PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

include $(INCLUDE_DIR)/package.mk

define Package/fuse24/Default
  TITLE:=FUSE for kernel 2.4
  DEPENDS:=@LINUX_2_4
  URL:=http://fuse.sourceforge.net/
endef

define Package/fuse24/Default/description
 FUSE for kernel 2.4 (Filesystem in UserSpacE) contains only the kernel module. 
 The libraries and utilities can be compiled from a recent FUSE version (package fuse).
 For details see:
 http://apps.sourceforge.net/mediawiki/fuse/index.php?title=FAQ#What_version_of_FUSE_do_I_need_to_use_FUSE_with_Linux_2.4.3F
endef

define KernelPackage/fuse24
$(call Package/fuse24/Default)
  SUBMENU:=Filesystems
  DEPENDS:=@LINUX_2_4
  TITLE+= (kernel module)
  FILES:=$(PKG_INSTALL_DIR)/lib/modules/$(LINUX_VERSION)/kernel/fs/fuse/fuse.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,80,fuse) 
endef

define KernelPackage/fuse24/description
$(call Package/fuse24/Default/description)
 This package contains the FUSE kernel 2.4 module.
endef

define KernelPackage/fuse24/install
	$(call $(Package/kmod-fuse24/install))
	echo mknod /dev/fuse -m 0666 c 10 229 >> $(1)/CONTROL/postinst
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); \
		rm -rf config.{cache,status} ; \
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
			--disable-example \
			--disable-auto-modprobe \
			--with-kernel=$(LINUX_DIR) \
			--disable-mtab \
			--disable-lib \
			--disable-util \
	);
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include
	$(CP)	$(PKG_INSTALL_DIR)/usr/include/fuse{,.h} $(1)/usr/include/
#	mkdir -p $(1)/usr/lib
#	$(CP)	$(PKG_INSTALL_DIR)/usr/lib/libfuse.{a,so*} $(1)/usr/lib/
	mkdir -p $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/fuse.pc $(1)/usr/lib/pkgconfig/
	$(SED) 's,-I$$$${includedir}/fuse,,g' $(1)/usr/lib/pkgconfig/fuse.pc
	$(SED) 's,-L$$$${libdir},,g' $(1)/usr/lib/pkgconfig/fuse.pc
	#echo mknod /dev/fuse -m 0666 c 10 229 >> $(PKG_BUILD_DIR)/ipkg/kmod-fuse24/CONTROL/postinst
endef

$(eval $(call KernelPackage,fuse24))

# 
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=fuse
PKG_VERSION:=2.7.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=4879f06570d2225667534c37fea04213

include $(INCLUDE_DIR)/package.mk

define Package/fuse/Default
  TITLE:=FUSE
  URL:=http://fuse.sourceforge.net/
endef

define Package/fuse/Default/description
 FUSE (Filesystem in UserSpacE)
endef

define Package/fuse-utils
$(call Package/fuse/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libfuse
  TITLE+= (utilities)
  SUBMENU:=filesystem
endef

define Package/fuse-utils/description
$(call Package/fuse/Default/description)
 This package contains the FUSE utility 'fusermount'.
endef

define KernelPackage/fuse
$(call Package/fuse/Default)
  SUBMENU:=Filesystems
  DEPENDS:=@LINUX_2_6
  TITLE+= (kernel module)
  KCONFIG:= CONFIG_FUSE_FS
  FILES:=$(LINUX_DIR)/fs/fuse/fuse.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,80,fuse)
  VERSION:=$(LINUX_VERSION)
endef

define KernelPackage/fuse/description
$(call Package/fuse/Default/description)
 This package contains the FUSE kernel module.
endef

define Package/libfuse
$(call Package/fuse/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= (library)
  DEPENDS:=+LINUX_2_4:kmod-fuse24 +LINUX_2_6:kmod-fuse
  SUBMENU:=filesystem
endef

define Package/libfuse/description
$(call Package/fuse/Default/description)
 This package contains the FUSE shared library, needed by other programs.
endef

CONFIGURE_VARS += \
	 kernsrcver="$(LINUX_VERSION)"

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-rpath \
	--enable-lib \
	--enable-util \
	--disable-example \
	--disable-auto-modprobe \
	--with-kernel="$(LINUX_DIR)" \
	--disable-mtab 

# This section is disabled; since kernel 2.6.14 the fuse module is officially 
# part of the kernel. The following enables this fuse packages kernel module 
# instead of the kernel's, if the kernel is younger than 2.6.25. 
# Generally it is better to use the kernel's module. Because it is DCACHE BUG 
# patched and from fuse 2.8.0 on it will be removed from the fuse package 
# anyway.
#
#ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),lt,2.6.25)),1)
#  define KernelPackage/fuse/2.6
#    KCONFIG:=
#    FILES:=$(PKG_INSTALL_DIR)/lib/modules/$(LINUX_VERSION)/kernel/fs/fuse/fuse.$(LINUX_KMOD_SUFFIX)
#  endef
#
# CONFIGURE_ARGS += --enable-kernel-module
#else
# CONFIGURE_ARGS += --disable-kernel-module
#endif

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
		AM_CFLAGS="$(TARGET_CFLAGS) -DDISABLE_COMPAT=1" \
		EXTRA_DIST="" \
		all install
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include
	$(CP)	$(PKG_INSTALL_DIR)/usr/include/fuse{,.h} $(1)/usr/include/
	mkdir -p $(1)/usr/lib
	$(CP)	$(PKG_INSTALL_DIR)/usr/lib/libfuse.{a,so*} $(1)/usr/lib/
	mkdir -p $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/fuse.pc $(1)/usr/lib/pkgconfig/
	$(SED) 's,-I$$$${includedir}/fuse,,g' $(1)/usr/lib/pkgconfig/fuse.pc
	$(SED) 's,-L$$$${libdir},,g' $(1)/usr/lib/pkgconfig/fuse.pc
endef

define Package/fuse-utils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/fusermount $(1)/usr/bin/
endef

define Package/libfuse/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfuse.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,fuse-utils))
$(eval $(call BuildPackage,libfuse))
$(eval $(call KernelPackage,fuse))

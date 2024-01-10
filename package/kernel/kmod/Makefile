#
# Copyright (C) 2015 Jeff Waugh
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=kmod
PKG_VERSION:=31
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@KERNEL/linux/utils/kernel/kmod
PKG_HASH:=f5a6949043cc72c001b728d8c218609c5a15f3c33d75614b78c79418fcf00d80

PKG_MAINTAINER:=Jeff Waugh <jdub@bethesignal.org>
PKG_LICENSE:=LGPL-2.1-or-later
PKG_LICENSE_FILES:=COPYING

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += --with-zlib

HOST_CONFIGURE_VARS += ./configure --with-zlib

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) tools/kmod
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/tools/kmod $(STAGING_DIR_HOST)/bin/kmod
	ln -sf kmod $(STAGING_DIR_HOST)/bin/depmod
	ln -sf kmod $(STAGING_DIR_HOST)/bin/modinfo
endef

define Host/Clean
	$(Host/Clean/Default)
	rm -f $(STAGING_DIR_HOST)/bin/kmod
	rm -f $(STAGING_DIR_HOST)/bin/depmod
	rm -f $(STAGING_DIR_HOST)/bin/modinfo
endef

define Package/kmod/Default
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Linux kernel module handling
  URL:=https://www.kernel.org/pub/linux/utils/kernel/kmod/
  DEPENDS:=+zlib
endef


define Package/kmod
$(call Package/kmod/Default)
  TITLE+= (tools)
  ALTERNATIVES:=\
    200:/sbin/depmod:/sbin/kmod \
    200:/sbin/insmod:/sbin/kmod \
    200:/sbin/lsmod:/sbin/kmod \
    200:/sbin/modinfo:/sbin/kmod \
    200:/sbin/modprobe:/sbin/kmod \
    200:/sbin/rmmod:/sbin/kmod
endef

define Package/kmod/description
Linux kernel module handling
 kmod is a set of tools to handle common tasks with Linux kernel modules like
 insert, remove, list, check properties, resolve dependencies and aliases.
endef

define Package/kmod/install
	$(INSTALL_DIR) $(1)/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/kmod $(1)/sbin
endef


define Package/libkmod
$(call Package/kmod/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= (library)
endef

define Package/libkmod/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libkmod.so.* $(1)/usr/lib/
endef


define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include $(1)/usr/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libkmod.{so*,la} $(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libkmod.pc $(1)/usr/lib/pkgconfig/
	$(SED) 's,/usr/include,$$$${prefix}/include,g' $(1)/usr/lib/pkgconfig/libkmod.pc
	$(SED) 's,/usr/lib,$$$${exec_prefix}/lib,g' $(1)/usr/lib/pkgconfig/libkmod.pc
endef


$(eval $(call HostBuild))
$(eval $(call BuildPackage,kmod))
$(eval $(call BuildPackage,libkmod))

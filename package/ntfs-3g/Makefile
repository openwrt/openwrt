# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ntfs-3g
PKG_VERSION:=1.616
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://www.ntfs-3g.org/
PKG_MD5SUM:=fdb1c55f00263c962a52d0707a6d075e

PKG_BUILD_DEPENDS:=libfuse

include $(INCLUDE_DIR)/package.mk

define Package/ntfs-3g
  TITLE:=NTFS-3G
  DEPENDS:=+libfuse +fuse-utils
  SECTION:=utils
  CATEGORY:=Utilities
  URL:=http://www.ntfs-3g.org
endef

define Package/ntfs-3g/description
 This package contains the third generation Read/Write NTFS driver
endef

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	, \
	ac_cv_path_LDCONFIG=""

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Build/InstallDev
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP)	$(PKG_INSTALL_DIR)/usr/include/ntfs-3g $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP)	$(PKG_INSTALL_DIR)/usr/lib/libntfs-3g.{a,so*} $(STAGING_DIR)/usr/lib/
endef

define Build/UninstallDev
	rm -rf	$(STAGING_DIR)/usr/include/ntfs-3g \
		$(STAGING_DIR)/usr/lib/libntfs-3g.{a,so*}
endef

define Package/ntfs-3g/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ntfs-3g $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libntfs-3g.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,ntfs-3g))

$(eval $(call RequireCommand,pkg-config, \
	$(PKG_NAME) requires pkg-config. \
))

#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=opkg
PKG_REV:=503
PKG_VERSION:=$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=opkg-$(PKG_VERSION)
PKG_SOURCE_URL:=http://opkg.googlecode.com/svn/trunk/
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_FIXUP = libtool
include $(INCLUDE_DIR)/package.mk

define Package/opkg
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=opkg package management system
  URL:=http://wiki.openmoko.org/wiki/Opkg
endef

define Package/opkg/description
  Lightweight package management system
  opkg is the opkg Package Management System, for handling
  installation and removal of packages on a system. It can
  recursively follow dependencies and download all packages
  necessary to install a particular package.

  opkg knows how to install both .ipk and .deb packages.
endef

define Package/opkg/conffiles
/etc/opkg.conf
endef

TARGET_CFLAGS += $(if $(CONFIG_GCC_VERSION_4_3)$(CONFIG_GCC_VERSION_4_4),-Wno-array-bounds)
EXTRA_CFLAGS += $(TARGET_CPPFLAGS)

CONFIGURE_ARGS += \
	--disable-curl \
	--disable-gpg \
	--with-ipkgetcdir=/etc

define Build/Configure
	(cd $(PKG_BUILD_DIR); \
		autoreconf -v --install || exit 1 \
	);
	$(call Build/Configure/Default)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		HOST_CPU="$(PKGARCH)" \
		all install
endef

define Package/opkg/install
	$(INSTALL_DIR) $(1)/usr/lib/opkg
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/opkg.conf $(1)/etc/
	$(SED) 's,$$$$S,$(PKGARCH),g' $(1)/etc/opkg.conf
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/opkg-cl $(1)/bin/opkg
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libopkg $(1)/usr/include/
endef

$(eval $(call BuildPackage,opkg))

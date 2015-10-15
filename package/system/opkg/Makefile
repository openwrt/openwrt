#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/version.mk
include $(INCLUDE_DIR)/feeds.mk

PKG_NAME:=opkg
PKG_REV:=9c97d5ecd795709c8584e972bfdf3aee3a5b846d
PKG_VERSION:=$(PKG_REV)
PKG_RELEASE:=9

PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=opkg-$(PKG_VERSION)
PKG_SOURCE_URL:=http://git.yoctoproject.org/git/opkg
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)
PKG_FIXUP:=autoreconf
PKG_REMOVE_FILES = autogen.sh aclocal.m4

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=COPYING

PKG_CONFIG_DEPENDS := CONFIG_SIGNED_PACKAGES

PKG_BUILD_PARALLEL:=1
HOST_BUILD_PARALLEL:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define Package/opkg/Default
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=opkg package manager
  MAINTAINER:=Jo-Philipp Wich <xm@subsignal.org>
  URL:=http://wiki.openmoko.org/wiki/Opkg
endef

define Package/opkg/Default/description
  Lightweight package management system
  opkg is the opkg Package Management System, for handling
  installation and removal of packages on a system. It can
  recursively follow dependencies and download all packages
  necessary to install a particular package.

  opkg knows how to install both .ipk and .deb packages.
endef

define Package/opkg
  $(call Package/opkg/Default)
  VARIANT:=unsigned
endef

define Package/opkg/description
  $(call Package/opkg/Default/description)
endef

define Package/opkg/conffiles
/etc/opkg.conf
/etc/opkg/keys/
/etc/opkg/customfeeds.conf
endef


define Package/opkg-smime
  $(call Package/opkg/Default)
  TITLE+= (with S/MIME signature support)
  DEPENDS+=+libopenssl
  VARIANT:=smime
endef

define Package/opkg-smime/description
  $(call Package/opkg/Default/description)

  This package allows the Package index to be verified with S/MIME.
endef

Package/opkg-smime/conffiles = $(Package/opkg/conffiles)

TARGET_CFLAGS += $(if $(CONFIG_GCC_VERSION_4_3)$(CONFIG_GCC_VERSION_4_4),-Wno-array-bounds)
TARGET_CFLAGS += -ffunction-sections -fdata-sections
EXTRA_CFLAGS += $(TARGET_CPPFLAGS)

CONFIGURE_ARGS += \
	--disable-curl \
	--disable-gpg \
	--enable-sha256 \
	--with-opkgetcdir=/etc \
	--with-opkglockfile=/var/lock/opkg.lock

ifeq ($(BUILD_VARIANT),smime)
	CONFIGURE_ARGS += --enable-openssl --disable-usign
else
  ifndef CONFIG_SIGNED_PACKAGES
    CONFIGURE_ARGS += --disable-usign
  endif
endif

MAKE_FLAGS = \
		CC="$(TARGET_CC)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		HOST_CPU="$(PKGARCH)" \
		LDFLAGS="-Wl,--gc-sections" \

define Package/opkg/Default/install
	$(INSTALL_DIR) $(1)/usr/lib/opkg
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_DIR) $(1)/etc/opkg
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DATA) ./files/customfeeds.conf $(1)/etc/opkg/customfeeds.conf
	$(INSTALL_DATA) ./files/opkg$(2).conf $(1)/etc/opkg.conf
	$(call FeedSourcesAppend,$(1)/etc/opkg/distfeeds.conf)
	$(VERSION_SED) $(1)/etc/opkg/distfeeds.conf
	$(INSTALL_BIN) ./files/20_migrate-feeds $(1)/etc/uci-defaults/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/opkg-cl $(1)/bin/opkg
endef

define Package/opkg/install
	$(call Package/opkg/Default/install,$(1),)
  ifneq ($(CONFIG_SIGNED_PACKAGES),)
	echo "option check_signature 1" >> $(1)/etc/opkg.conf
  endif
	mkdir $(1)/usr/sbin
	$(INSTALL_BIN) ./files/opkg-key $(1)/usr/sbin/
endef

define Package/opkg-smime/install
	$(call Package/opkg/Default/install,$(1),-smime)
	$(INSTALL_DIR) $(1)/etc/ssl/certs
	$(if $(CONFIG_OPKGSMIME_CERT),$(INSTALL_DATA) $(call qstrip,$(CONFIG_OPKGSMIME_CERT)) $(1)/etc/ssl/certs/opkg.pem,)
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libopkg $(1)/usr/include/
endef


HOST_CONFIGURE_ARGS+= \
	--disable-curl \
	--disable-gpg \
	--enable-sha256 \
	--with-opkgetcdir=/etc \
	--with-opkglockfile=/tmp/opkg.lock

define Host/Compile
	+$(MAKE) $(HOST_JOBS) -C $(HOST_BUILD_DIR) CC="$(HOSTCC)" all
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/src/opkg-cl $(STAGING_DIR_HOST)/bin/opkg
endef

$(eval $(call BuildPackage,opkg))
$(eval $(call BuildPackage,opkg-smime))
$(eval $(call HostBuild))

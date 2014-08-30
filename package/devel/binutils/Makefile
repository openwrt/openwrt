#
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=binutils
PKG_VERSION:=2.24
PKG_RELEASE:=2

PKG_SOURCE_URL:=@GNU/binutils
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_VERSION:=$(PKG_VERSION)
PKG_MD5SUM:=e0f71a7b2ddab0f8612336ac81d9636b

PKG_FIXUP:=autoreconf
PKG_LIBTOOL_PATHS:=. gas bfd opcodes gprof binutils ld libiberty gold intl
PKG_REMOVE_FILES:=libtool.m4
PKG_INSTALL:=1

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/binutils
  SECTION:=devel
  CATEGORY:=Development
  TITLE:=binutils
  DEPENDS:=+objdump
endef

define Package/objdump
  SECTION:=devel
  CATEGORY:=Development
  TITLE:=objdump
  DEPENDS:=+zlib 
endef

define Package/binutils/description
  The Binutils package contains a linker, an assembler, and other tools for handling object files
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-install-libiberty \
	--enable-install-libbfd

define Build/Install
	$(call Build/Install/Default)
	$(MAKE) -C $(PKG_BUILD_DIR)/libiberty \
		target_header_dir=libiberty \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		MULTIOSDIR="" \
		install
endef

define Build/InstallDev
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

define Package/objdump/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/objdump $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libopcodes*.so $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libbfd*.so $(1)/usr/lib/
endef

define Package/binutils/install
	$(INSTALL_DIR) $(1)/usr $(1)/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ $(1)/usr/
	mv $(1)/usr/bin/strings $(1)/bin/strings
	rm -f $(1)/usr/bin/objdump
endef

$(eval $(call BuildPackage,binutils))
$(eval $(call BuildPackage,objdump))

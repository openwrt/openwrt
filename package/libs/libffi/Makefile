#
# Copyright (C) 2009-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libffi
PKG_VERSION:=3.4.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/libffi/libffi/releases/download/v$(PKG_VERSION)
PKG_HASH:=138607dee268bdecf374adf9144c00e839e38541f75f24a1fcf18b78fda48b2d

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE
PKG_CPE_ID:=cpe:/a:libffi_project:libffi

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1
PKG_BUILD_FLAGS:=no-mips16

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define Package/libffi
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Foreign Function Interface (FFI) library
  URL:=https://sourceware.org/libffi/
  MAINTAINER:=Peter Wagner <tripolar@gmx.at>
endef

define Package/libffi/description
The libffi library provides a portable, high level programming interface to
various calling conventions. This allows a programmer to call any function
specified by a call interface description at run-time.

FFI stands for Foreign Function Interface. A foreign function interface is the
popular name for the interface that allows code written in one language to call
code written in another language. The libffi library really only provides the
lowest, machine dependent layer of a fully featured foreign function interface.
A layer must exist above libffi that handles type conversions for values passed
between the two languages.
endef

HOST_CONFIGURE_ARGS += \
	--disable-shared \
	--disable-docs \
	--disable-multi-os-directory \
	--disable-raw-api \
	--disable-structs \
	--with-pic

CONFIGURE_ARGS += \
	--disable-docs \
	--disable-multi-os-directory \
	--disable-raw-api \
	--disable-structs

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libffi.{so*,a,la} \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/* \
		$(1)/usr/lib/pkgconfig/
	$(SED) 's,includedir=.*,includedir=$$$${prefix}/include,' $(1)/usr/lib/pkgconfig/libffi.pc

	$(INSTALL_DIR) $(1)/usr/include
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/*.h \
		$(1)/usr/include/
endef

define Package/libffi/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libffi.so.* \
		$(1)/usr/lib/
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,libffi))

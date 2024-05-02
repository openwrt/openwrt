# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=elfutils
PKG_VERSION:=0.191
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://sourceware.org/$(PKG_NAME)/ftp/$(PKG_VERSION)
PKG_HASH:=df76db71366d1d708365fc7a6c60ca48398f14367eb2b8954efc8897147ad871

PKG_LICENSE:=GPL-3.0-or-later
PKG_LICENSE_FILES:=COPYING COPYING-GPLV2 COPYING-LGPLV3
PKG_CPE_ID:=cpe:/a:elfutils_project:elfutils

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

PKG_SUBDIRS := \
	libgnu \
	config \
	lib \
	libelf \
	libcpu \
	backends \
	libebl \
	libdwelf \
	libdwfl \
	libdw

PKG_GNULIB_BASE:=libgnu

PKG_GNULIB_ARGS = \
	--dir=$(HOST_BUILD_DIR) \
	--local-dir=$(STAGING_DIR_HOST)/share/gnulib \
	--source-base=$(PKG_GNULIB_BASE) \
	--libtool \
	--avoid=reallocarray \
	--import

PKG_GNULIB_MODS = \
	argp \
	fts \
	obstack \
	progname \
	strchrnul \
	tsearch

include $(INCLUDE_DIR)/host-build.mk

ifeq ($(HOST_OS),Darwin)
  HOST_CFLAGS += -I/opt/homebrew/include
endif

HOST_CFLAGS += -Wno-error -fPIC

HOST_CONFIGURE_ARGS += \
	--without-libintl-prefix \
	--without-libiconv-prefix \
	--disable-debuginfod \
	--disable-libdebuginfod \
	--disable-nls \
	--disable-shared \
	--enable-static \
	--without-lzma \
	--without-bzlib \
	--without-zstd

ifeq ($(HOST_OS),Darwin)
  HOST_CONFIGURE_ARGS += --disable-symbol-versioning
endif

Hooks/HostConfigure/Pre := Host/Gnulib $(Hooks/HostConfigure/Pre)
define Host/Gnulib
	$(STAGING_DIR_HOST)/bin/gnulib-tool $(PKG_GNULIB_ARGS) $(PKG_GNULIB_MODS);
	ln -sf ../lib/eu-config.h $(HOST_BUILD_DIR)/libgnu/;
endef

define Host/Compile
	$(call Host/Compile/Default,SUBDIRS='$$$$(wildcard $(PKG_SUBDIRS))')
endef

define Host/Install
	$(call Host/Compile/Default,install SUBDIRS='$$$$(wildcard $(PKG_SUBDIRS))')
endef

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))

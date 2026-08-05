# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=elfutils
PKG_VERSION:=0.192
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://sourceware.org/$(PKG_NAME)/ftp/$(PKG_VERSION)
PKG_HASH:=616099beae24aba11f9b63d86ca6cc8d566d968b802391334c91df54eab416b4

PKG_LICENSE:=GPL-3.0-or-later
PKG_LICENSE_FILES:=COPYING COPYING-GPLV2 COPYING-LGPLV3
PKG_CPE_ID:=cpe:/a:elfutils_project:elfutils

PKG_FIXUP:=autoreconf

PKG_PROGRAMS:=elflint findtextrel elfcmp unstrip stack elfcompress elfclassify srcfiles

PKG_SUBDIRS := \
	config \
	lib \
	libelf \
	libcpu \
	backends \
	libebl \
	libdwelf \
	libdwfl \
	libdw \
	src

PKG_GNULIB_BASE:=libgnu

PKG_GNULIB_ARGS = \
	--dir=$(HOST_BUILD_DIR) \
	--libtool \
	--avoid=reallocarray \
	--import

PKG_GNULIB_MODS = \
	argp \
	fallocate-posix \
	fnmatch-gnu \
	fts \
	obstack-printf-posix \
	progname \
	strchrnul \
	tsearch

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

export $(PKG_GNULIB_BASE)=$(HOST_BUILD_DIR)/$(PKG_GNULIB_BASE)/.libs/$(PKG_GNULIB_BASE).a

export $(PKG_GNULIB_BASE)_exitfail=$(HOST_BUILD_DIR)/$(PKG_GNULIB_BASE)/$(PKG_GNULIB_BASE)_la-exitfail.o
export $(PKG_GNULIB_BASE)_fallocate-posix=$(HOST_BUILD_DIR)/$(PKG_GNULIB_BASE)/$(PKG_GNULIB_BASE)_la-posix_fallocate.o
export $(PKG_GNULIB_BASE)_mbszero=$(HOST_BUILD_DIR)/$(PKG_GNULIB_BASE)/$(PKG_GNULIB_BASE)_la-mbszero.o
export $(PKG_GNULIB_BASE)_obstack=$(HOST_BUILD_DIR)/$(PKG_GNULIB_BASE)/$(PKG_GNULIB_BASE)_la-obstack.o
export $(PKG_GNULIB_BASE)_obstack-printf=$(HOST_BUILD_DIR)/$(PKG_GNULIB_BASE)/$(PKG_GNULIB_BASE)_la-obstack_printf.o
export $(PKG_GNULIB_BASE)_printf-args=$(HOST_BUILD_DIR)/$(PKG_GNULIB_BASE)/$(PKG_GNULIB_BASE)_la-printf-args.o
export $(PKG_GNULIB_BASE)_printf-frexp=$(HOST_BUILD_DIR)/$(PKG_GNULIB_BASE)/$(PKG_GNULIB_BASE)_la-printf-frexp.o
export $(PKG_GNULIB_BASE)_printf-frexpl=$(HOST_BUILD_DIR)/$(PKG_GNULIB_BASE)/$(PKG_GNULIB_BASE)_la-printf-frexpl.o
export $(PKG_GNULIB_BASE)_printf-parse=$(HOST_BUILD_DIR)/$(PKG_GNULIB_BASE)/$(PKG_GNULIB_BASE)_la-printf-parse.o
export $(PKG_GNULIB_BASE)_tsearch=$(HOST_BUILD_DIR)/$(PKG_GNULIB_BASE)/$(PKG_GNULIB_BASE)_la-tsearch.o
export $(PKG_GNULIB_BASE)_vasnprintf=$(HOST_BUILD_DIR)/$(PKG_GNULIB_BASE)/$(PKG_GNULIB_BASE)_la-vasnprintf.o
export $(PKG_GNULIB_BASE)_xsize=$(HOST_BUILD_DIR)/$(PKG_GNULIB_BASE)/$(PKG_GNULIB_BASE)_la-xsize.o

HOST_MAKE_FLAGS += \
	am__CONFIG_DISTCLEAN_FILES= \
	DEFAULT_INCLUDES='-iquote . -I$$$$(top_builddir) -I$$$$(top_srcdir)/$(PKG_GNULIB_BASE)' \
	AM_LDFLAGS='$$$$(STACK_USAGE_NO_ERROR)' \
	LIBS+='$$$$(if $$$$(findstring $(lastword $(PKG_SUBDIRS)),$$$$(subdir)), $$$$($(PKG_GNULIB_BASE)))' \
	LIBS+='$$$$(wildcard $$$$($(PKG_GNULIB_BASE)_exitfail))' \
	LIBS+='$$$$(wildcard $$$$($(PKG_GNULIB_BASE)_fallocate-posix))' \
	LIBS+='$$$$(wildcard $$$$($(PKG_GNULIB_BASE)_mbszero))' \
	LIBS+='$$$$(wildcard $$$$($(PKG_GNULIB_BASE)_obstack))' \
	LIBS+='$$$$(wildcard $$$$($(PKG_GNULIB_BASE)_obstack-printf))' \
	LIBS+='$$$$(wildcard $$$$($(PKG_GNULIB_BASE)_printf-args))' \
	LIBS+='$$$$(wildcard $$$$($(PKG_GNULIB_BASE)_printf-frexp))' \
	LIBS+='$$$$(wildcard $$$$($(PKG_GNULIB_BASE)_printf-frexpl))' \
	LIBS+='$$$$(wildcard $$$$($(PKG_GNULIB_BASE)_printf-parse))' \
	LIBS+='$$$$(wildcard $$$$($(PKG_GNULIB_BASE)_tsearch))' \
	LIBS+='$$$$(wildcard $$$$($(PKG_GNULIB_BASE)_vasnprintf))' \
	LIBS+='$$$$(wildcard $$$$($(PKG_GNULIB_BASE)_xsize))' \
	REPLACE_FCNTL=0 REPLACE_FREE=0 REPLACE_FSTAT=0 REPLACE_OPEN=0 \
	REPLACE_OBSTACK=0 REPLACE_OBSTACK_PRINTF=0 \
	bin_PROGRAMS='$(PKG_PROGRAMS)' EXEEXT=

HOST_CPPFLAGS += "'-I$$$$(top_srcdir)/lib'"

ifeq ($(HOST_OS),Darwin)
  HOST_CFLAGS += -I/opt/homebrew/include
endif

HOST_CFLAGS += -Wno-error -fPIC -std=gnu17

HOST_CONFIGURE_ARGS += \
	--enable-maintainer-mode \
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

HOST_CONFIGURE_VARS += \
	ac_cv_search_argp_parse=yes \
	ac_cv_search_fts_close=yes \
	ac_cv_search__obstack_free=yes \
	ac_cv_buildid=yes

# Host-build fix for glibc >= 2.41 (e.g. Ubuntu 25.10): gnulib-tool imports a
# ctype module whose ctype.in.h -> ctype.h generation leaves the @GNULIB_*@
# substitutions empty, producing broken "#if ( || )" / empty-"#if" preprocessor
# lines and failing the libgnu compile. The host's system <ctype.h> is complete,
# so replace the imported ctype.in.h with a passthrough. Runs in HostConfigure/Pre
# right after the gnulib import; the rewritten ctype.in.h persists to the later
# libgnu compile (Gnulib/Compile), where ctype.h is generated from it. Idempotent;
# a no-op if the ctype module was not imported.
define Host/CtypeFix
	[ ! -e $(HOST_BUILD_DIR)/$(PKG_GNULIB_BASE)/ctype.in.h ] || \
	printf '%s\n' \
		'/* OpenWrt host-build fix for glibc >= 2.41: pass through to the system ctype.h */' \
		'#ifndef _GL_CTYPE_H' \
		'#if __GNUC__ >= 3' \
		'#pragma GCC system_header' \
		'#endif' \
		'#include_next <ctype.h>' \
		'#define _GL_CTYPE_H' \
		'#endif' \
		> $(HOST_BUILD_DIR)/$(PKG_GNULIB_BASE)/ctype.in.h ;
endef

Hooks/HostConfigure/Pre := Host/Gnulib/Prepare Host/CtypeFix $(Hooks/HostConfigure/Pre)
Hooks/HostCompile/Pre := Host/Gnulib/Compile $(Hooks/HostCompile/Pre)

define Host/Configure
	$(call Host/Configure/Default)
	$(call Host/Uninstall)
endef

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
	-$(call Host/Compile/Default,maintainer-clean) # Clean bootstrap files from the release
endef

$(eval $(call HostBuild))

#
# Copyright (C) 2006-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=binutils
PKG_VERSION:=$(call qstrip,$(CONFIG_BINUTILS_VERSION))
BASE_VERSION:=$(subst $(space),.,$(wordlist 1,2,$(subst .,$(space),$(PKG_VERSION))))

PKG_SOURCE_URL:=@GNU/binutils/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_CPE_ID:=cpe:/a:gnu:binutils

TAR_OPTIONS += --exclude='*.rej'

ifeq ($(PKG_VERSION),2.44)
  PKG_HASH:=ce2017e059d63e67ddb9240e9d4ec49c2893605035cd60e92ad53177f4377237
endif

ifeq ($(PKG_VERSION),2.45.1)
  PKG_HASH:=5fe101e6fe9d18fdec95962d81ed670fdee5f37e3f48f0bef87bddf862513aa5
endif

ifeq ($(PKG_VERSION),2.46.0)
  PKG_HASH:=d75a94f4d73e7a4086f7513e67e439e8fcdcbb726ffe63f4661744e6256b2cf2
endif

HOST_BUILD_PARALLEL:=1

PATCH_DIR:=./patches/$(BASE_VERSION)

include $(INCLUDE_DIR)/toolchain-build.mk

export ZSTD_CFLAGS=-I$(STAGING_DIR_HOST)/include -pthread
export ZSTD_LIBS=-L$(STAGING_DIR_HOST)/lib -lzstd -lpthread

ifdef CONFIG_GCC_USE_GRAPHITE
  GRAPHITE_CONFIGURE:= --with-isl=$(STAGING_DIR_HOST)
else
  GRAPHITE_CONFIGURE:= --without-isl --without-cloog
endif

HOST_CONFIGURE_ARGS = \
	--prefix=$(TOOLCHAIN_DIR) \
	--build=$(GNU_HOST_NAME) \
	--host=$(GNU_HOST_NAME) \
	--target=$(REAL_GNU_TARGET_NAME) \
	--with-sysroot=$(TOOLCHAIN_DIR) \
	--enable-dependency-tracking \
	--enable-serial-configure \
	--enable-obsolete \
	--enable-deterministic-archives \
	--enable-plugins \
	--enable-lto \
	--disable-werror \
	--disable-gprofng \
	--disable-multilib \
	--disable-nls \
	--disable-sim \
	--disable-gdb \
	--with-system-zlib \
	--with-zstd \
	--without-debuginfod \
	$(GRAPHITE_CONFIGURE) \
	$(SOFT_FLOAT_CONFIG_OPTION) \
	$(call qstrip,$(CONFIG_EXTRA_BINUTILS_CONFIG_OPTIONS))

ifneq ($(CONFIG_SSP_SUPPORT),)
  HOST_CONFIGURE_ARGS+= \
		--enable-libssp
else
  HOST_CONFIGURE_ARGS+= \
		--disable-libssp
endif

ifneq ($(CONFIG_EXTRA_TARGET_ARCH),)
  HOST_CONFIGURE_ARGS+= \
		--enable-targets=$(call qstrip,$(CONFIG_EXTRA_TARGET_ARCH_NAME))-linux-$(TARGET_SUFFIX)
endif

BINUTILS_VARS = \
	ac_cv_path_EXPECT= \
	ac_cv_path_GMSGFMT=: \
	ac_cv_path_MSGFMT=: \
	ac_cv_path_MSGMERGE=: \
	ac_cv_path_XGETTEXT=: \
	ac_cv_prog_ACLOCAL=$(STAGING_DIR_HOST)/bin/aclocal \
	ac_cv_prog_AUTOCONF=$(STAGING_DIR_HOST)/bin/autoconf \
	ac_cv_prog_AUTOHEADER=$(STAGING_DIR_HOST)/bin/autoheader \
	ac_cv_prog_AUTOMAKE=$(STAGING_DIR_HOST)/bin/automake \
	ac_cv_prog_CARGO= \
	ac_cv_prog_EXPECT= \
	ac_cv_prog_GDC= \
	ac_cv_prog_GNATBIND= \
	ac_cv_prog_GNATMAKE= \
	ac_cv_prog_MAKEINFO= \
	ac_cv_prog_ac_ct_GDC= \
	ac_cv_prog_ac_ct_GNATBIND= \
	ac_cv_prog_ac_ct_GNATMAKE= \
	ac_cv_dejagnu_compat=no \
	ac_cv_libctf_tcl_try=no \
	acx_cv_cc_gcc_supports_ada=no \
	acx_cv_d_compiler_works=no \


HOST_CONFIGURE_VARS += $(BINUTILS_VARS)

HOST_MAKE_VARS += $(BINUTILS_VARS)

define Host/Prepare
	$(call Host/Prepare/Default)
	ln -snf $(notdir $(HOST_BUILD_DIR)) $(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
	$(CP) $(SCRIPT_DIR)/config.{guess,sub} $(HOST_BUILD_DIR)/
endef

define Host/Compile
	+$(HOST_MAKE_VARS) \
	$(MAKE) $(HOST_JOBS) -C $(HOST_BUILD_DIR) all
endef

define Host/Install
	$(MAKE) -C $(HOST_BUILD_DIR) \
		install
	$(call FixupLibdir,$(TOOLCHAIN_DIR))
	$(CP) $(TOOLCHAIN_DIR)/bin/$(REAL_GNU_TARGET_NAME)-readelf $(HOST_BUILD_PREFIX)/bin/readelf
endef

define Host/Clean
	rm -rf \
		$(HOST_BUILD_DIR) \
		$(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
endef

$(eval $(call HostBuild))

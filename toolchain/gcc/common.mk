#
# Copyright (C) 2002-2003 Erik Andersen <andersen@uclibc.org>
# Copyright (C) 2004 Manuel Novoa III <mjn3@uclibc.org>
# Copyright (C) 2005-2006 Felix Fietkau <nbd@openwrt.org>
# Copyright (C) 2006-2012 OpenWrt.org
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

include $(TOPDIR)/rules.mk

PKG_NAME:=gcc
GCC_VERSION:=$(call qstrip,$(CONFIG_GCC_VERSION))
PKG_VERSION:=$(firstword $(subst +, ,$(GCC_VERSION)))
GCC_DIR:=$(PKG_NAME)-$(PKG_VERSION)

ifdef CONFIG_GCC_VERSION_LLVM
  PKG_SOURCE_VERSION:=c98c494b72ff875884c0c7286be67f16f9f6d7ab
  PKG_REV:=83504
  GCC_DIR:=llvm-gcc-4.2-r$(PKG_REV)
  PKG_VERSION:=4.2.1
  PKG_SOURCE:=$(GCC_DIR).tar.gz
  PKG_SOURCE_PROTO:=git
  PKG_SOURCE_URL:=git://repo.or.cz/llvm-gcc-4.2.git
  PKG_SOURCE_SUBDIR:=$(GCC_DIR)
  HOST_BUILD_DIR:=$(BUILD_DIR_TOOLCHAIN)/$(GCC_DIR)
else
ifeq ($(findstring linaro, $(CONFIG_GCC_VERSION)),linaro)
    ifeq ($(CONFIG_GCC_VERSION),"4.5-linaro")
      PKG_REV:=4.5-2012.02
      PKG_VERSION:=4.5.4
      PKG_VERSION_MAJOR:=4.5
      PKG_MD5SUM:=e05be9ea8eca2ad4c859d35dbab568e7
    endif
    ifeq ($(CONFIG_GCC_VERSION),"4.6-linaro")
      PKG_REV:=4.6-2012.02
      PKG_VERSION:=4.6.3
      PKG_VERSION_MAJOR:=4.6
      PKG_MD5SUM:=2b7887846f8e5ac1ca58fe4dfaabf5a6
    endif
    ifeq ($(CONFIG_GCC_VERSION),"4.7-linaro")
      PKG_REV:=4.7-2012.04
      PKG_VERSION:=4.7.1
      PKG_VERSION_MAJOR:=4.7
      PKG_MD5SUM:=6dab459c1177fc9ae2969e7a39549d44
    endif
    PKG_SOURCE_URL:=http://launchpad.net/gcc-linaro/$(PKG_VERSION_MAJOR)/$(PKG_REV)/+download/
    PKG_SOURCE:=$(PKG_NAME)-linaro-$(PKG_REV).tar.bz2
    GCC_DIR:=gcc-linaro-$(PKG_REV)
    HOST_BUILD_DIR:=$(BUILD_DIR_TOOLCHAIN)/$(GCC_DIR)
else
  PKG_SOURCE_URL:=@GNU/gcc/gcc-$(PKG_VERSION)
  PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2

  ifeq ($(PKG_VERSION),4.4.7)
    PKG_MD5SUM:=295709feb4441b04e87dea3f1bab4281
  endif
  ifeq ($(PKG_VERSION),4.6.2)
    PKG_MD5SUM:=028115c4fbfb6cfd75d6369f4a90d87e
  endif
  ifeq ($(PKG_VERSION),4.7.0)
    PKG_MD5SUM:=2a0f1d99fda235c29d40b561f81d9a77
  endif
endif
endif

PATCH_DIR=../patches/$(GCC_VERSION)

BUGURL=https://dev.openwrt.org/

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/toolchain-build.mk

HOST_SOURCE_DIR:=$(HOST_BUILD_DIR)
ifeq ($(GCC_VARIANT),minimal)
  GCC_BUILD_DIR:=$(HOST_BUILD_DIR)-$(GCC_VARIANT)
else
  HOST_BUILD_DIR:=$(HOST_BUILD_DIR)-$(GCC_VARIANT)
  GCC_BUILD_DIR:=$(HOST_BUILD_DIR)
endif

HOST_STAMP_PREPARED:=$(HOST_BUILD_DIR)/.prepared
HOST_STAMP_BUILT:=$(GCC_BUILD_DIR)/.built
HOST_STAMP_CONFIGURED:=$(GCC_BUILD_DIR)/.configured
HOST_STAMP_INSTALLED:=$(STAGING_DIR_HOST)/stamp/.gcc_$(GCC_VARIANT)_installed

SEP:=,
TARGET_LANGUAGES:="c$(if $(CONFIG_INSTALL_LIBSTDCPP),$(SEP)c++)$(if $(CONFIG_INSTALL_LIBGCJ),$(SEP)java)$(if $(CONFIG_INSTALL_GFORTRAN),$(SEP)fortran)"

export libgcc_cv_fixed_point=no
ifdef CONFIG_USE_UCLIBC
  export glibcxx_cv_c99_math_tr1=no
endif

GCC_CONFIGURE:= \
	SHELL="$(BASH)" \
	$(HOST_SOURCE_DIR)/configure \
		--prefix=$(TOOLCHAIN_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--with-gnu-ld \
		--enable-target-optspace \
		--disable-libgomp \
		--disable-libmudflap \
		--disable-multilib \
		--disable-nls \
		$(GRAPHITE_CONFIGURE) \
		--with-host-libstdcxx=-lstdc++ \
		$(SOFT_FLOAT_CONFIG_OPTION) \
		$(call qstrip,$(CONFIG_EXTRA_GCC_CONFIG_OPTIONS)) \
		$(if $(CONFIG_mips64)$(CONFIG_mips64el),--with-arch=mips64 --with-abi=64) \
		$(if $(CONFIG_GCC_VERSION_LLVM),--enable-llvm=$(BUILD_DIR_BASE)/host/llvm) \

ifeq ($(CONFIG_GCC_LLVM),)
  GCC_BUILD_TARGET_LIBGCC:=y
  GCC_CONFIGURE+= \
		--with-gmp=$(TOPDIR)/staging_dir/host \
		--with-mpfr=$(TOPDIR)/staging_dir/host \
		--disable-decimal-float
  ifneq ($(CONFIG_mips)$(CONFIG_mipsel),)
    GCC_CONFIGURE += --with-mips-plt
  endif
endif

ifneq ($(CONFIG_GCC_VERSION_4_5)$(CONFIG_GCC_VERSION_4_6),)
  GCC_CONFIGURE+= \
		--with-mpc=$(TOPDIR)/staging_dir/host
endif

ifneq ($(CONFIG_SSP_SUPPORT),)
  GCC_CONFIGURE+= \
		--enable-libssp
else
  GCC_CONFIGURE+= \
		--disable-libssp
endif

ifneq ($(CONFIG_EXTRA_TARGET_ARCH),)
  GCC_CONFIGURE+= \
		--enable-biarch \
		--enable-targets=$(call qstrip,$(CONFIG_EXTRA_TARGET_ARCH_NAME))-linux-$(TARGET_SUFFIX)
endif

ifdef CONFIG_sparc
  GCC_CONFIGURE+= --enable-targets=all
endif

ifeq ($(LIBC),uClibc)
  GCC_CONFIGURE+= \
		--disable-__cxa_atexit
else
  GCC_CONFIGURE+= \
		--enable-__cxa_atexit
endif

ifneq ($(GCC_ARCH),)
  GCC_CONFIGURE+= --with-arch=$(GCC_ARCH)
endif

GCC_MAKE:= \
	export SHELL="$(BASH)"; \
	$(MAKE) $(TOOLCHAIN_JOBS) \
		CFLAGS_FOR_TARGET="$(TARGET_CFLAGS)" \
		CXXFLAGS_FOR_TARGET="$(TARGET_CFLAGS)"

define Host/Prepare
	mkdir -p $(GCC_BUILD_DIR)
endef

define Host/Configure
	(cd $(GCC_BUILD_DIR) && rm -f config.cache; \
		$(GCC_CONFIGURE) \
	);
endef

define Host/Clean
	rm -rf \
		$(STAGING_DIR_HOST)/stamp/.gcc_* \
		$(STAGING_DIR_HOST)/stamp/.binutils_* \
		$(GCC_BUILD_DIR) \
		$(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME) \
		$(TOOLCHAIN_DIR)/$(REAL_GNU_TARGET_NAME) \
		$(TOOLCHAIN_DIR)/bin/$(REAL_GNU_TARGET_NAME)-gc* \
		$(TOOLCHAIN_DIR)/bin/$(REAL_GNU_TARGET_NAME)-c*
endef

# Makefile for to build a gcc/uClibc toolchain
#
# Copyright (C) 2002-2003 Erik Andersen <andersen@uclibc.org>
# Copyright (C) 2004 Manuel Novoa III <mjn3@uclibc.org>
# Copyright (C) 2005-2006 Felix Fietkau <nbd@openwrt.org>
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
PKG_VERSION:=$(call qstrip,$(CONFIG_GCC_VERSION))

PKG_SOURCE_URL:=@GNU/gcc/gcc-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2

ifeq ($(PKG_VERSION),3.4.6)
  PKG_MD5SUM:=4a21ac777d4b5617283ce488b808da7b
endif
ifeq ($(PKG_VERSION),4.1.2)
  PKG_MD5SUM:=a4a3eb15c96030906d8494959eeda23c
endif
ifeq ($(PKG_VERSION),4.2.4)
  PKG_MD5SUM:=d79f553e7916ea21c556329eacfeaa16
endif

PATCH_DIR=./patches/$(PKG_VERSION)

STAGING_DIR_HOST:=$(TOOLCHAIN_DIR)
BUILD_DIR_HOST:=$(BUILD_DIR_TOOLCHAIN)
override CONFIG_AUTOREBUILD=

include $(INCLUDE_DIR)/host-build.mk

STAMP_BUILT:=$(TOOLCHAIN_DIR)/stamp/.gcc-initial_installed
BUILD_DIR1:=$(BUILD_DIR_HOST)/gcc-$(PKG_VERSION)-initial
BUILD_DIR2:=$(BUILD_DIR_HOST)/gcc-$(PKG_VERSION)-final

SEP:=,
TARGET_LANGUAGES:="c$(if $(CONFIG_INSTALL_LIBSTDCPP),$(SEP)c++)$(if $(CONFIG_INSTALL_LIBGCJ),$(SEP)java)"

ifeq ($(CONFIG_SSP_SUPPORT),y)
  SSP:=--enable-ssp
else
  SSP:=--disable-ssp
endif

EXTRA_TARGET=$(if $(CONFIG_EXTRA_TARGET_ARCH),--enable-biarch --enable-targets=$(call qstrip,$(CONFIG_EXTRA_TARGET_ARCH_NAME))-linux-uclibc)

define Stage1/Configure
	$(SED) 's,TARGET_CROSS=.*,TARGET_CROSS=$(REAL_GNU_TARGET_NAME)-,' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,GCC_VERSION=.*,GCC_VERSION=$(PKG_VERSION),' $(TOOLCHAIN_DIR)/info.mk
	mkdir -p $(BUILD_DIR1)
	(cd $(BUILD_DIR1); rm -f config.cache; \
		SHELL="$(BASH)" \
		$(PKG_BUILD_DIR)/configure \
		--prefix=$(TOOLCHAIN_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--enable-languages=c \
		--disable-shared \
		--with-sysroot=$(BUILD_DIR_HOST)/uClibc_dev/ \
		--disable-__cxa_atexit \
		--enable-target-optspace \
		--with-gnu-ld \
		--disable-nls \
		--disable-libmudflap \
		--disable-multilib \
		$(SSP) \
		$(EXTRA_TARGET) \
		$(SOFT_FLOAT_CONFIG_OPTION) \
		$(call qstrip,$(CONFIG_EXTRA_GCC_CONFIG_OPTIONS)) \
	);
endef
define Stage1/Compile
	export SHELL="$(BASH)"; $(MAKE) -C $(BUILD_DIR1) all-gcc
endef
define Stage1/Install
	export SHELL="$(BASH)"; $(MAKE) -C $(BUILD_DIR1) install-gcc
endef

define Stage2/Configure
	mkdir -p $(BUILD_DIR2) $(TOOLCHAIN_DIR)/$(REAL_GNU_TARGET_NAME)
	# Important!  Required for limits.h to be fixed.
	rm -rf $(TOOLCHAIN_DIR)/$(REAL_GNU_TARGET_NAME)/sys-include
	ln -sf ../include $(TOOLCHAIN_DIR)/$(REAL_GNU_TARGET_NAME)/sys-include
	rm -rf $(TOOLCHAIN_DIR)/$(REAL_GNU_TARGET_NAME)/lib
	ln -sf ../lib $(TOOLCHAIN_DIR)/$(REAL_GNU_TARGET_NAME)/lib
	(cd $(BUILD_DIR2); rm -f config.cache; \
		SHELL="$(BASH)" \
		$(PKG_BUILD_DIR)/configure \
		--prefix=$(TOOLCHAIN_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--enable-languages=$(TARGET_LANGUAGES) \
		--enable-shared \
		--disable-__cxa_atexit \
		--enable-target-optspace \
		--with-gnu-ld \
		--disable-nls \
		--disable-libmudflap \
		--disable-multilib \
		$(SSP) \
		$(EXTRA_TARGET) \
		$(SOFT_FLOAT_CONFIG_OPTION) \
		$(call qstrip,$(CONFIG_EXTRA_GCC_CONFIG_OPTIONS)) \
	);
endef

define Stage2/Compile
	export SHELL="\$(BASH)"; \$(MAKE) -C \$(BUILD_DIR2) all
endef

define SetupExtraArch
	for app in $(TOOLCHAIN_DIR)/bin/$(OPTIMIZE_FOR_CPU)*-{gcc,gcc-*,g++}; do \
		[ -e $$$$app ] || continue; \
		old_base=$$$$(basename $$$$app); \
		new_base=$(call qstrip,$(CONFIG_EXTRA_TARGET_ARCH_NAME))-$$$${old_base##$(OPTIMIZE_FOR_CPU)-}; \
		sed -e "s/@CC_BASE@/$$$$old_base/" \
			-e 's/@EXTRA_ARCH_OPTS@/$(call qstrip,$(CONFIG_EXTRA_TARGET_ARCH_OPTS))/' \
			 ./files/alternate-arch-cc.in > \
			 $(TOOLCHAIN_DIR)/bin/$$$$new_base; \
		chmod a+x $(TOOLCHAIN_DIR)/bin/$$$$new_base; \
	done
endef

define Stage2/Install
	$(MAKE) -C $(BUILD_DIR2) \
		SHELL="$(BASH)" \
		install
	# Set up the symlinks to enable lying about target name.
	set -e; \
	(cd $(TOOLCHAIN_DIR); \
		ln -sf $(REAL_GNU_TARGET_NAME) $(GNU_TARGET_NAME); \
		cd bin; \
		for app in $(REAL_GNU_TARGET_NAME)-* ; do \
			ln -sf $$$${app} \
		   	$(GNU_TARGET_NAME)$$$${app##$(REAL_GNU_TARGET_NAME)}; \
		done; \
	);
	$(if $(CONFIG_EXTRA_TARGET_ARCH),$(call SetupExtraArch))
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(SED) 's,\(version_string.. = "[0-9\.]*\).*\(";\),\1 (OpenWrt-2.0)\2,' $(PKG_BUILD_DIR)/gcc/version.c
	$(SED) 's,\(bug_report_url.. = "\).*\(";\),\1<URL:https://dev.openwrt.org/>\2,' $(PKG_BUILD_DIR)/gcc/version.c
	(cd $(PKG_BUILD_DIR)/libstdc++-v3; autoconf;);
	$(SED) 's,gcc_no_link=yes,gcc_no_link=no,' $(PKG_BUILD_DIR)/libstdc++-v3/configure
endef

define Build/Configure
	$(call Stage1/Configure)
endef

define Build/Compile
	$(call Stage1/Compile)
	$(if $(wildcard $(TOOLCHAIN_DIR)/bin/$(GNU_TARGET_NAME)-gcc),,$(call Stage1/Install))
endef

define Build/Install
	$(call Stage2/Configure)
	$(call Stage2/Compile)
	$(call Stage2/Install)
endef

define Build/Clean
	rm -rf \
		$(PKG_BUILD_DIR) \
		$(BUILD_DIR1) \
		$(BUILD_DIR2) \
		$(TOOLCHAIN_DIR)/$(REAL_GNU_TARGET_NAME) \
		$(TOOLCHAIN_DIR)/bin/$(REAL_GNU_TARGET_NAME)-gc* \
		$(TOOLCHAIN_DIR)/bin/$(REAL_GNU_TARGET_NAME)-c*
endef

$(eval $(call HostBuild))

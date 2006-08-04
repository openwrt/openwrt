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
PKG_VERSION:=$(strip $(subst ",, $(CONFIG_GCC_VERSION)))#"))

PKG_SOURCE:=gcc-$(PKG_VERSION).tar.bz2
PKG_MD5SUM:=unknown
PKG_SOURCE_URL:=ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-$(PKG_VERSION) \
	http://mirrors.rcn.net/pub/sourceware/gcc/releases/gcc-$(PKG_VERSION) \
	ftp://ftp.gnu.org/gnu/gcc/releases/gcc-$(PKG_VERSION)
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(TOOL_BUILD_DIR)/gcc-$(PKG_VERSION)

TARGET_LANGUAGES:=c
ifeq ($(CONFIG_INSTALL_LIBSTDCPP),y)
TARGET_LANGUAGES:=$(TARGET_LANGUAGES),c++
endif
ifeq ($(CONFIG_INSTALL_LIBGCJ),y)
TARGET_LANGUAGES:=$(TARGET_LANGUAGES),java
endif

include $(INCLUDE_DIR)/host-build.mk

BUILD_DIR1:=$(TOOL_BUILD_DIR)/gcc-$(PKG_VERSION)-initial
BUILD_DIR2:=$(TOOL_BUILD_DIR)/gcc-$(PKG_VERSION)-final


define Stage1/Configure
	mkdir -p $(BUILD_DIR1)
	(cd $(BUILD_DIR1); rm -f config.cache; \
		$(PKG_BUILD_DIR)/configure \
		--prefix=$(STAGING_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--enable-languages=c \
		--disable-shared \
		--with-sysroot=$(TOOL_BUILD_DIR)/uClibc_dev/ \
		--disable-__cxa_atexit \
		--enable-target-optspace \
		--with-gnu-ld \
		--disable-nls \
	);
endef
define Stage1/Compile
	$(MAKE) -C $(BUILD_DIR1) all-gcc
endef
define Stage1/Install
	$(MAKE) -C $(BUILD_DIR1) install-gcc
endef

define Stage2/Configure
	mkdir -p $(BUILD_DIR2)
	# Important!  Required for limits.h to be fixed.
	rm -rf $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/sys-include
	ln -sf ../include $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/sys-include
	rm -rf $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/lib
	ln -sf ../lib $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/lib
	(cd $(BUILD_DIR2); rm -f config.cache; \
		$(PKG_BUILD_DIR)/configure \
		--prefix=$(STAGING_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--enable-languages=$(TARGET_LANGUAGES) \
		--enable-shared \
		--disable-__cxa_atexit \
		--enable-target-optspace \
		--with-gnu-ld \
		--disable-nls \
	);
endef
define Stage2/Compile
	$(MAKE) -C $(BUILD_DIR2) all
endef
define Stage2/Install
	$(MAKE) -C $(BUILD_DIR2) install
	echo $(PKG_VERSION) > $(STAGING_DIR)/gcc_version
	# Set up the symlinks to enable lying about target name.
	set -e; \
	(cd $(STAGING_DIR); \
		ln -sf $(REAL_GNU_TARGET_NAME) $(GNU_TARGET_NAME); \
		cd bin; \
		for app in $(REAL_GNU_TARGET_NAME)-* ; do \
			ln -sf $${app} \
		   	$(GNU_TARGET_NAME)$${app##$(REAL_GNU_TARGET_NAME)}; \
		done; \
	);
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(SCRIPT_DIR)/patch-kernel.sh $(PKG_BUILD_DIR) ./patches/$(PKG_VERSION) \*.patch
	$(SED) 's,\(version_string.. = "[0-9\.]*\).*\(";\),\1 (OpenWrt-2.0)\2,' $(PKG_BUILD_DIR)/gcc/version.c
	$(SED) 's,\(bug_report_url.. = "\).*\(";\),\1<URL:https://dev.openwrt.org/>\2,' $(PKG_BUILD_DIR)/gcc/version.c
endef

define Build/Configure
	$(call Stage1/Configure)
endef

define Build/Compile
	$(call Stage1/Compile)
	$(call Stage1/Install)
endef

define Build/Install
	$(call Stage2/Configure)
	$(call Stage2/Compile)
	$(call Stage2/Install)
endef

define Build/Clean
	rm -rf $(PKG_BUILD_DIR)
	rm -rf $(BUILD_DIR1)
	rm -rf $(BUILD_DIR2)
	rm -f $(STAGING_DIR)/bin/$(REAL_GNU_TARGET_NAME)-gc*
	rm -f $(STAGING_DIR)/bin/$(REAL_GNU_TARGET_NAME)-c*
endef

$(eval $(call HostBuild))

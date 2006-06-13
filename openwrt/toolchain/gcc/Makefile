# Makefile for to build a gcc/uClibc toolchain
#
# Copyright (C) 2002-2003 Erik Andersen <andersen@uclibc.org>
# Copyright (C) 2004 Manuel Novoa III <mjn3@uclibc.org>
# Copyright (C) 2005 Felix Fietkau <openwrt@nbd.name>
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

GCC_VERSION:=$(strip $(subst ",, $(CONFIG_GCC_VERSION)))#"))

#GCC_SITE:=ftp://ftp.gnu.org/gnu/gcc/releases/gcc-$(GCC_VERSION)
GCC_SITE:=ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-$(GCC_VERSION) \
	http://mirrors.rcn.net/pub/sourceware/gcc/releases/gcc-$(GCC_VERSION)

GCC_SOURCE:=gcc-$(GCC_VERSION).tar.bz2
GCC_DIR:=$(TOOL_BUILD_DIR)/gcc-$(GCC_VERSION)
GCC_CAT:=bzcat

#############################################################
#
# Setup some initial stuff
#
#############################################################

ifeq ($(CONFIG_INSTALL_LIBGCJ),y)
TARGET_LANGUAGES:=c,c++,java
else
ifeq ($(CONFIG_INSTALL_LIBSTDCPP),y)
TARGET_LANGUAGES:=c,c++
else
TARGET_LANGUAGES:=c
endif
endif

#############################################################
#
# build the first pass gcc compiler
#
#############################################################

GCC_BUILD_DIR1:=$(TOOL_BUILD_DIR)/gcc-$(GCC_VERSION)-initial

$(DL_DIR)/$(GCC_SOURCE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(GCC_SOURCE) x $(GCC_SITE)

$(GCC_DIR)/.unpacked: $(DL_DIR)/$(GCC_SOURCE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(GCC_CAT) $(DL_DIR)/$(GCC_SOURCE) | tar -C $(TOOL_BUILD_DIR) $(TAR_OPTIONS) -
	touch $(GCC_DIR)/.unpacked

$(GCC_DIR)/.patched: $(GCC_DIR)/.unpacked
	# Apply any files named gcc-*.patch from the source directory to gcc
	$(SCRIPT_DIR)/patch-kernel.sh $(GCC_DIR) ./$(GCC_VERSION) \*.patch
	# Note: The soft float situation has improved considerably with gcc 3.4.x.
	# We can dispense with the custom spec files, as well as libfloat for the arm case.
	# However, we still need a patch for arm.  There's a similar patch for gcc 3.3.x
	# which needs to be integrated so we can kill of libfloat for good.
	$(SED) 's,\(version_string.. = "[0-9\.]*\).*\(";\),\1 (OpenWrt-2.0)\2,' $(GCC_DIR)/gcc/version.c
	$(SED) 's,\(bug_report_url.. = "\).*\(";\),\1<URL:https://dev.openwrt.org/>\2,' $(GCC_DIR)/gcc/version.c
	touch $(GCC_DIR)/.patched

# The --without-headers option stopped working with gcc 3.0 and has never been
# # fixed, so we need to actually have working C library header files prior to
# # the step or libgcc will not build...

$(GCC_BUILD_DIR1)/.configured: $(GCC_DIR)/.patched
	mkdir -p $(GCC_BUILD_DIR1)
	(cd $(GCC_BUILD_DIR1); rm -f config.cache; PATH=$(TARGET_PATH) \
		$(GCC_DIR)/configure \
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
		$(DISABLE_NLS) \
		$(MULTILIB) \
	);
	touch $(GCC_BUILD_DIR1)/.configured

$(GCC_BUILD_DIR1)/.compiled: $(GCC_BUILD_DIR1)/.configured
	PATH=$(TARGET_PATH) $(MAKE) -C $(GCC_BUILD_DIR1) all-gcc
	touch $(GCC_BUILD_DIR1)/.compiled

$(STAGING_DIR)/bin/$(REAL_GNU_TARGET_NAME)-gcc: $(GCC_BUILD_DIR1)/.compiled
	PATH=$(TARGET_PATH) $(MAKE) -C $(GCC_BUILD_DIR1) install-gcc

#############################################################
#
# second pass compiler build.  Build the compiler targeting 
# the newly built shared uClibc library.
#
#############################################################
#
# Sigh... I had to rework things because using --with-gxx-include-dir
# causes issues with include dir search order for g++.  This seems to
# have something to do with "path translations" and possibly doesn't
# affect gcc-target.  However, I haven't tested gcc-target yet so no
# guarantees.  mjn3

GCC_BUILD_DIR2:=$(TOOL_BUILD_DIR)/gcc-$(GCC_VERSION)-final
$(GCC_BUILD_DIR2)/.configured: $(GCC_DIR)/.patched $(STAGING_DIR)/lib/libc.a
	mkdir -p $(GCC_BUILD_DIR2)
	# Important!  Required for limits.h to be fixed.
	rm -rf $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/sys-include
	ln -sf ../include $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/sys-include
	rm -rf $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/lib
	ln -sf ../lib $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/lib
	(cd $(GCC_BUILD_DIR2); rm -f config.cache; PATH=$(TARGET_PATH) \
		$(GCC_DIR)/configure \
		--prefix=$(STAGING_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--enable-languages=$(TARGET_LANGUAGES) \
		--enable-shared \
		--disable-__cxa_atexit \
		--enable-target-optspace \
		--with-gnu-ld \
		$(DISABLE_NLS) \
		$(MULTILIB) \
	);
	touch $(GCC_BUILD_DIR2)/.configured

$(GCC_BUILD_DIR2)/.compiled: $(GCC_BUILD_DIR2)/.configured
	PATH=$(TARGET_PATH) $(MAKE) -C $(GCC_BUILD_DIR2) all
	touch $(GCC_BUILD_DIR2)/.compiled

gcc-install: $(GCC_BUILD_DIR2)/.compiled FORCE
	PATH=$(TARGET_PATH) $(MAKE) -C $(GCC_BUILD_DIR2) install
	echo $(GCC_VERSION) > $(STAGING_DIR)/gcc_version
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

source: $(DL_DIR)/$(GCC_SOURCE)
prepare: $(STAGING_DIR)/bin/$(REAL_GNU_TARGET_NAME)-gcc
compile: gcc-install
install:
clean: gcc-clean
	rm -rf $(GCC_DIR)
	rm -rf $(GCC_BUILD_DIR1)
	rm -rf $(GCC_BUILD_DIR2)
	rm -f $(STAGING_DIR)/bin/$(REAL_GNU_TARGET_NAME)-gc*
	rm -f $(STAGING_DIR)/bin/$(REAL_GNU_TARGET_NAME)-c*

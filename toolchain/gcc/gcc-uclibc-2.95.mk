# Makefile for to build a gcc/uClibc toolchain
#
# Copyright (C) 2002-2003 Erik Andersen <andersen@uclibc.org>
# Copyright (C) 2004 Manuel Novoa III <mjn3@uclibc.org>
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

ifeq ($(findstring 2.95,$(GCC_VERSION)),2.95)
GCC_VERSION:=$(strip $(GCC_VERSION))

GCC_SITE:=http://www.uclibc.org/downloads/toolchain
GCC_SOURCE:=gcc-20011006.tar.bz2
GCC_DIR:=$(TOOL_BUILD_DIR)/gcc-20011006
GCC_CAT:=bzcat

STLPORT_SITE=http://www.stlport.org/archive
STLPORT_SOURCE=STLport-4.5.3.tar.gz
STLPORT_DIR=$(TOOL_BUILD_DIR)/STLport-4.5.3
GCC_STRIP_HOST_BINARIES:=true

#############################################################
#
# Setup some initial stuff
#
#############################################################

ifeq ($(BR2_INSTALL_LIBSTDCPP),y)
TARGET_LANGUAGES:=c,c++
STLPORT_TARGET=stlport
else
TARGET_LANGUAGES:=c
STLPORT_TARGET=
endif

#############################################################
#
# build the first pass gcc compiler
#
#############################################################
GCC_BUILD_DIR1:=$(TOOL_BUILD_DIR)/gcc-$(GCC_VERSION)-initial

$(DL_DIR)/$(GCC_SOURCE):
	mkdir -p $(DL_DIR)
	$(WGET) -P $(DL_DIR) $(GCC_SITE)/$(GCC_SOURCE)

$(GCC_DIR)/.unpacked: $(DL_DIR)/$(GCC_SOURCE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(GCC_CAT) $(DL_DIR)/$(GCC_SOURCE) | tar -C $(TOOL_BUILD_DIR) $(TAR_OPTIONS) -
	touch $(GCC_DIR)/.unpacked

$(GCC_DIR)/.patched: $(GCC_DIR)/.unpacked
	# Apply any files named gcc-*.patch from the source directory to gcc
	toolchain/patch-kernel.sh $(GCC_DIR) toolchain/gcc/$(GCC_VERSION) \*.patch\*
	#
	# We do not wish to build the libstdc++ library provided with gcc,
	# since it doesn't seem to work at all with uClibc plus gcc 2.95...
	#
	mv $(GCC_DIR)/libstdc++ $(GCC_DIR)/libstdc++.orig
	mv $(GCC_DIR)/libio $(GCC_DIR)/libio.orig
	#
	touch $(GCC_DIR)/.patched

# The --without-headers option stopped working with gcc 3.0 and has never been
# # fixed, so we need to actually have working C library header files prior to
# # the step or libgcc will not build...
$(GCC_BUILD_DIR1)/.configured: $(GCC_DIR)/.patched
	mkdir -p $(GCC_BUILD_DIR1)
	-mkdir -p $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/include
	# Important!  Required for limits.h to be fixed.
	ln -sf include $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/sys-include
	(cd $(GCC_BUILD_DIR1); PATH=$(TARGET_PATH) \
		$(GCC_DIR)/configure \
		--prefix=$(STAGING_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--enable-languages=c \
		--disable-shared \
		--includedir=$(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/include \
		--with-headers=$(TOOL_BUILD_DIR)/uClibc_dev/usr/include \
		--disable-__cxa_atexit \
		--enable-target-optspace \
		--with-gnu-ld \
		$(DISABLE_NLS) \
		$(MULTILIB) \
		$(SOFT_FLOAT_CONFIG_OPTION) \
		$(EXTRA_GCC_CONFIG_OPTIONS));
	touch $(GCC_BUILD_DIR1)/.configured

$(GCC_BUILD_DIR1)/.compiled: $(GCC_BUILD_DIR1)/.configured
	PATH=$(TARGET_PATH) $(MAKE) -C $(GCC_BUILD_DIR1) all-gcc
	touch $(GCC_BUILD_DIR1)/.compiled

$(STAGING_DIR)/bin/$(REAL_GNU_TARGET_NAME)-gcc: $(GCC_BUILD_DIR1)/.compiled
	PATH=$(TARGET_PATH) $(MAKE) -C $(GCC_BUILD_DIR1) install-gcc
	#rm -f $(STAGING_DIR)/bin/gccbug $(STAGING_DIR)/bin/gcov
	#rm -rf $(STAGING_DIR)/info $(STAGING_DIR)/man $(STAGING_DIR)/share/doc $(STAGING_DIR)/share/locale

gcc_initial: uclibc-configured binutils $(STAGING_DIR)/bin/$(REAL_GNU_TARGET_NAME)-gcc

gcc_initial-clean:
	rm -rf $(GCC_BUILD_DIR1)
	rm -f $(STAGING_DIR)/bin/$(REAL_GNU_TARGET_NAME)*

gcc_initial-dirclean:
	rm -rf $(GCC_BUILD_DIR1)

#############################################################
#
# STLport -- an alternative C++ library
#
#############################################################
STLPORT_PATCH=toolchain/gcc/2.95/STLport-4.5.3.patch

$(DL_DIR)/$(STLPORT_SOURCE):
	$(WGET) -P $(DL_DIR) $(STLPORT_SITE)/$(STLPORT_SOURCE)

$(STLPORT_DIR)/Makefile: $(DL_DIR)/$(STLPORT_SOURCE) $(STLPORT_PATCH)
	zcat $(DL_DIR)/$(STLPORT_SOURCE) | tar -C $(TOOL_BUILD_DIR) $(TAR_OPTIONS) - 
	cat $(STLPORT_PATCH) | patch -d $(STLPORT_DIR) -p1

$(STLPORT_DIR)/lib/libstdc++.a: $(STLPORT_DIR)/Makefile
	$(MAKE) ARCH=$(OPTIMIZE_FOR_CPU) PREFIX=$(STAGING_DIR)/$(REAL_GNU_TARGET_NAME) -C $(STLPORT_DIR)

$(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/lib/libstdc++.a: $(STLPORT_DIR)/lib/libstdc++.a
	$(MAKE) ARCH=$(OPTIMIZE_FOR_CPU) PREFIX=$(STAGING_DIR)/$(REAL_GNU_TARGET_NAME) -C $(STLPORT_DIR) install

stlport: $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/lib/libstdc++.a

stlport-source: $(DL_DIR)/$(STLPORT_SOURCE)

stlport-clean:
	rm -f $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/lib/libstdc++*
	rm -f $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/include/c++*
	-$(MAKE) -C $(STLPORT_DIR) clean

stlport-dirclean:
	rm -f $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/lib/libstdc++*
	rm -f $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/include/g++-v3*
	rm -rf $(STLPORT_DIR)

#############################################################
#
# second pass compiler build.  Build the compiler targeting 
# the newly built shared uClibc library.
#
#############################################################
GCC_BUILD_DIR2:=$(TOOL_BUILD_DIR)/gcc-$(GCC_VERSION)-final

$(GCC_BUILD_DIR2)/.configured: $(GCC_DIR)/.patched $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/lib/libc.a
	mkdir -p $(GCC_BUILD_DIR2)
	(cd $(GCC_BUILD_DIR2); PATH=$(TARGET_PATH) \
		$(GCC_DIR)/configure \
		--prefix=$(STAGING_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--enable-languages=$(TARGET_LANGUAGES) \
		--enable-shared \
		--with-gxx-include-dir=$(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/include/c++ \
		--disable-__cxa_atexit \
		--enable-target-optspace \
		--with-gnu-ld \
		$(DISABLE_NLS) \
		$(MULTILIB) \
		$(SOFT_FLOAT_CONFIG_OPTION) \
		$(EXTRA_GCC_CONFIG_OPTIONS));
	touch $(GCC_BUILD_DIR2)/.configured

$(GCC_BUILD_DIR2)/.compiled: $(GCC_BUILD_DIR2)/.configured
	PATH=$(TARGET_PATH) $(MAKE) -C $(GCC_BUILD_DIR2) all
	touch $(GCC_BUILD_DIR2)/.compiled

$(GCC_BUILD_DIR2)/.installed: $(GCC_BUILD_DIR2)/.compiled
	PATH=$(TARGET_PATH) $(MAKE) -C $(GCC_BUILD_DIR2) install
	# Strip the host binaries
ifeq ($(GCC_STRIP_HOST_BINARIES),true)
	-strip --strip-all -R .note -R .comment $(STAGING_DIR)/bin/*
endif
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
	touch $(GCC_BUILD_DIR2)/.installed

gcc: uclibc-configured binutils gcc_initial $(LIBFLOAT_TARGET) uclibc \
	$(GCC_BUILD_DIR2)/.installed $(GCC_TARGETS) $(STLPORT_TARGET)

gcc-source: $(DL_DIR)/$(GCC_SOURCE)

gcc-clean:
	rm -rf $(GCC_BUILD_DIR2)
	rm -f $(STAGING_DIR)/bin/$(REAL_GNU_TARGET_NAME)*

gcc-dirclean:
	rm -rf $(GCC_BUILD_DIR2)

#############################################################
#
# Next build target gcc compiler
#
#############################################################
GCC_BUILD_DIR3:=$(BUILD_DIR)/gcc-$(GCC_VERSION)-target

$(GCC_BUILD_DIR3)/.configured: $(GCC_BUILD_DIR2)/.installed
	mkdir -p $(GCC_BUILD_DIR3)
	(cd $(GCC_BUILD_DIR3); PATH=$(TARGET_PATH) \
		$(GCC_DIR)/configure \
		--prefix=/usr \
		--build=$(GNU_HOST_NAME) \
		--host=$(REAL_GNU_TARGET_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--enable-languages=$(TARGET_LANGUAGES) \
		--enable-shared \
		--with-gxx-include-dir=/usr/include/c++ \
		--disable-__cxa_atexit \
		--enable-target-optspace \
		--with-gnu-ld \
		$(DISABLE_NLS) \
		$(MULTILIB) \
		$(SOFT_FLOAT_CONFIG_OPTION) \
		$(EXTRA_GCC_CONFIG_OPTIONS));
	touch $(GCC_BUILD_DIR3)/.configured

$(GCC_BUILD_DIR3)/.compiled: $(GCC_BUILD_DIR3)/.configured
	PATH=$(TARGET_PATH) \
	$(MAKE) $(TARGET_GCC_ARGS) -C $(GCC_BUILD_DIR3) all
	touch $(GCC_BUILD_DIR3)/.compiled

$(TARGET_DIR)/usr/bin/gcc: $(GCC_BUILD_DIR3)/.compiled
	PATH=$(TARGET_PATH) \
	$(MAKE) DESTDIR=$(TARGET_DIR) -C $(GCC_BUILD_DIR3) install
	# Remove broken specs file (cross compile flag is set).
	rm -f $(TARGET_DIR)/usr/lib/gcc-lib/$(REAL_GNU_TARGET_NAME)/$(GCC_VERSION)/specs
	-(cd $(TARGET_DIR)/bin; find -type f | xargs $(STRIP) > /dev/null 2>&1)
	-(cd $(TARGET_DIR)/usr/bin; find -type f | xargs $(STRIP) > /dev/null 2>&1)
	-(cd $(TARGET_DIR)/usr/lib/gcc-lib/$(REAL_GNU_TARGET_NAME)/$(GCC_VERSION); $(STRIP) cc1 cc1plus collect2 > /dev/null 2>&1)
	-(cd $(TARGET_DIR)/usr/lib; $(STRIP) libstdc++.so.*.*.* > /dev/null 2>&1)
	-(cd $(TARGET_DIR)/lib; $(STRIP) libgcc_s.so.*.*.* > /dev/null 2>&1)
	#
	rm -f $(TARGET_DIR)/usr/lib/*.la*
	#rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
	#	$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc
	# Work around problem of missing syslimits.h
	cp -f $(STAGING_DIR)/usr/lib/gcc-lib/$(REAL_GNU_TARGET_NAME)/$(GCC_VERSION)/include/syslimits.h $(TARGET_DIR)/usr/lib/gcc-lib/$(REAL_GNU_TARGET_NAME)/$(GCC_VERSION)/include/
	# These are in /lib, so...
	#rm -rf $(TARGET_DIR)/usr/lib/libgcc_s.so*
	#touch -c $(TARGET_DIR)/usr/bin/gcc

gcc_target: uclibc_target binutils_target $(TARGET_DIR)/usr/bin/gcc

gcc_target-clean:
	rm -rf $(GCC_BUILD_DIR3)
	rm -f $(TARGET_DIR)/bin/$(REAL_GNU_TARGET_NAME)*

gcc_target-dirclean:
	rm -rf $(GCC_BUILD_DIR3)

endif

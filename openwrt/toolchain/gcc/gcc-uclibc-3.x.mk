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

ifneq ($(findstring 2.95,$(GCC_VERSION)),2.95)
GCC_VERSION:=$(strip $(GCC_VERSION))

#GCC_SITE:=ftp://ftp.gnu.org/gnu/gcc/releases/gcc-$(GCC_VERSION)
GCC_SITE:=http://mirrors.rcn.net/pub/sourceware/gcc/releases/gcc-$(GCC_VERSION)

#
# snapshots....
#GCC_VERSION:=3.3-20031013
#GCC_SITE:=http://gcc.get-software.com/snapshots/$(GCC_VERSION)
#
GCC_SOURCE:=gcc-$(GCC_VERSION).tar.bz2
GCC_DIR:=$(TOOL_BUILD_DIR)/gcc-$(GCC_VERSION)
GCC_CAT:=bzcat
GCC_STRIP_HOST_BINARIES:=true

#############################################################
#
# Setup some initial stuff
#
#############################################################

ifeq ($(BR2_INSTALL_LIBGCJ),y)
TARGET_LANGUAGES:=c,c++,java
else
ifeq ($(BR2_INSTALL_LIBSTDCPP),y)
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
	$(WGET) -P $(DL_DIR) $(GCC_SITE)/$(GCC_SOURCE)

$(GCC_DIR)/.unpacked: $(DL_DIR)/$(GCC_SOURCE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(GCC_CAT) $(DL_DIR)/$(GCC_SOURCE) | tar -C $(TOOL_BUILD_DIR) $(TAR_OPTIONS) -
	touch $(GCC_DIR)/.unpacked

$(GCC_DIR)/.patched: $(GCC_DIR)/.unpacked
	# Apply any files named gcc-*.patch from the source directory to gcc
	toolchain/patch-kernel.sh $(GCC_DIR) toolchain/gcc/$(GCC_VERSION) \*.patch
	# Note: The soft float situation has improved considerably with gcc 3.4.x.
	# We can dispense with the custom spec files, as well as libfloat for the arm case.
	# However, we still need a patch for arm.  There's a similar patch for gcc 3.3.x
	# which needs to be integrated so we can kill of libfloat for good, except for
	# anyone (?) who might still be using gcc 2.95.  mjn3
ifeq ($(BR2_SOFT_FLOAT),y)
ifeq ("$(strip $(ARCH))","arm")
	toolchain/patch-kernel.sh $(GCC_DIR) toolchain/gcc/$(GCC_VERSION) arm-softfloat.patch.conditional
endif
ifeq ("$(strip $(ARCH))","armeb")
	toolchain/patch-kernel.sh $(GCC_DIR) toolchain/gcc/$(GCC_VERSION) arm-softfloat.patch.conditional
endif
	# Not yet updated to 3.4.1.
	#ifeq ("$(strip $(ARCH))","i386")
	#toolchain/patch-kernel.sh $(GCC_DIR) toolchain/gcc i386-gcc-soft-float.patch
	#endif
endif
	touch $(GCC_DIR)/.patched

# The --without-headers option stopped working with gcc 3.0 and has never been
# # fixed, so we need to actually have working C library header files prior to
# # the step or libgcc will not build...

$(GCC_BUILD_DIR1)/.configured: $(GCC_DIR)/.patched
	mkdir -p $(GCC_BUILD_DIR1)
	(cd $(GCC_BUILD_DIR1); PATH=$(TARGET_PATH) \
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
		$(SOFT_FLOAT_CONFIG_OPTION) \
		$(GCC_WITH_CPU) $(GCC_WITH_ARCH) $(GCC_WITH_TUNE) \
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
	ln -sf ../include $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/sys-include
	(cd $(GCC_BUILD_DIR2); PATH=$(TARGET_PATH) \
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
		$(SOFT_FLOAT_CONFIG_OPTION) \
		$(GCC_WITH_CPU) $(GCC_WITH_ARCH) $(GCC_WITH_TUNE) \
		$(GCC_USE_SJLJ_EXCEPTIONS) \
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
	#
	# Now for the ugly 3.3.x soft float hack...
	#
ifeq ($(BR2_SOFT_FLOAT),y)
ifeq ($(findstring 3.3.,$(GCC_VERSION)),3.3.)
	# Make sure we have a soft float specs file for this arch
	if [ ! -f toolchain/gcc/$(GCC_VERSION)/specs-$(ARCH)-soft-float ] ; then \
		echo soft float configured but no specs file for this arch ; \
		/bin/false ; \
	fi;
	# Replace specs file with one that defaults to soft float mode.
	if [ ! -f $(STAGING_DIR)/lib/gcc-lib/$(REAL_GNU_TARGET_NAME)/$(GCC_VERSION)/specs ] ; then \
		echo staging dir specs file is missing ; \
		/bin/false ; \
	fi;
	cp toolchain/gcc/$(GCC_VERSION)/specs-$(ARCH)-soft-float $(STAGING_DIR)/lib/gcc-lib/$(REAL_GNU_TARGET_NAME)/$(GCC_VERSION)/specs
endif
endif
	#
	# Ok... that's enough of that.
	#
	touch $(GCC_BUILD_DIR2)/.installed

$(TARGET_DIR)/lib/libgcc_s.so.1: $(GCC_BUILD_DIR2)/.installed
	# These are in /lib, so...
	rm -rf $(TARGET_DIR)/usr/lib/libgcc_s.so*
	-$(STRIP) $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/lib/libgcc_s.so.1
	-cp -a $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/lib/libgcc_s* $(TARGET_DIR)/lib/

gcc: uclibc-configured binutils gcc_initial $(LIBFLOAT_TARGET) uclibc \
	$(TARGET_DIR)/lib/libgcc_s.so.1 $(GCC_BUILD_DIR2)/.installed $(GCC_TARGETS)

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
		$(GCC_WITH_CPU) $(GCC_WITH_ARCH) $(GCC_WITH_TUNE) \
		$(GCC_USE_SJLJ_EXCEPTIONS) \
		$(EXTRA_GCC_CONFIG_OPTIONS));
	touch $(GCC_BUILD_DIR3)/.configured

$(GCC_BUILD_DIR3)/.compiled: $(GCC_BUILD_DIR3)/.configured
	PATH=$(TARGET_PATH) \
	$(MAKE) $(TARGET_GCC_ARGS) -C $(GCC_BUILD_DIR3) all
	touch $(GCC_BUILD_DIR3)/.compiled

#
# gcc-lib dir changes names to gcc with 3.4.mumble
#
ifeq ($(findstring 3.4.,$(GCC_VERSION)),3.4.)
GCC_LIB_SUBDIR=lib/gcc/$(REAL_GNU_TARGET_NAME)/$(GCC_VERSION)
else
GCC_LIB_SUBDIR=lib/gcc-lib/$(REAL_GNU_TARGET_NAME)/$(GCC_VERSION)
endif

$(TARGET_DIR)/usr/bin/gcc: $(GCC_BUILD_DIR3)/.compiled
	PATH=$(TARGET_PATH) \
	$(MAKE) DESTDIR=$(TARGET_DIR) -C $(GCC_BUILD_DIR3) install
	# Remove broken specs file (cross compile flag is set).
	rm -f $(TARGET_DIR)/usr/$(GCC_LIB_SUBDIR)/specs
	#
	# Now for the ugly 3.3.x soft float hack...
	#
ifeq ($(BR2_SOFT_FLOAT),y)
ifeq ($(findstring 3.3.,$(GCC_VERSION)),3.3.)
	# Add a specs file that defaults to soft float mode.
	cp toolchain/gcc/$(GCC_VERSION)/specs-$(ARCH)-soft-float $(TARGET_DIR)/usr/lib/gcc-lib/$(REAL_GNU_TARGET_NAME)/$(GCC_VERSION)/specs
	# Make sure gcc does not think we are cross compiling
	$(SED) "s/^1/0/;" $(TARGET_DIR)/usr/lib/gcc-lib/$(REAL_GNU_TARGET_NAME)/$(GCC_VERSION)/specs
endif
endif
	#
	# Ok... that's enough of that.
	#
	-(cd $(TARGET_DIR)/bin; find -type f | xargs $(STRIP) > /dev/null 2>&1)
	-(cd $(TARGET_DIR)/usr/bin; find -type f | xargs $(STRIP) > /dev/null 2>&1)
	-(cd $(TARGET_DIR)/usr/$(GCC_LIB_SUBDIR); $(STRIP) cc1 cc1plus collect2 > /dev/null 2>&1)
	-(cd $(TARGET_DIR)/usr/lib; $(STRIP) libstdc++.so.*.*.* > /dev/null 2>&1)
	-(cd $(TARGET_DIR)/lib; $(STRIP) libgcc_s.so.*.*.* > /dev/null 2>&1)
	#
	rm -f $(TARGET_DIR)/usr/lib/*.la*
	#rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
	#	$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc
	# Work around problem of missing syslimits.h
	@if [ ! -f $(TARGET_DIR)/usr/$(GCC_LIB_SUBDIR)/include/syslimits.h ] ; then \
		echo "warning: working around missing syslimits.h" ; \
		cp -f $(STAGING_DIR)/$(GCC_LIB_SUBDIR)/include/syslimits.h \
			$(TARGET_DIR)/usr/$(GCC_LIB_SUBDIR)/include/ ; \
	fi
	# These are in /lib, so...
	#rm -rf $(TARGET_DIR)/usr/lib/libgcc_s.so*
	#touch -c $(TARGET_DIR)/usr/bin/gcc

gcc_target: uclibc_target binutils_target $(TARGET_DIR)/usr/bin/gcc

gcc_target-clean:
	rm -rf $(GCC_BUILD_DIR3)
	rm -f $(TARGET_DIR)/usr/bin/$(REAL_GNU_TARGET_NAME)*

gcc_target-dirclean:
	rm -rf $(GCC_BUILD_DIR3)

endif

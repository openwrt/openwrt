# Makefile for a simple busybox/uClibc root filesystem
#
# Copyright (C) 2001-2004 Erik Andersen <andersen@codepoet.org>
# Copyright (C) 2002 by Tim Riker <Tim@Rikers.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA


#############################################################
#
# EDIT this stuff to suit your system and preferences
#
# Use := when possible to get precomputation, thereby
# speeding up the build process.
#
#############################################################

SNAPSHOT:=20040531

ARCH:=mipsel
USE_UCLIBC_SNAPSHOT:=$(SNAPSHOT)
USE_UCLIBC_LDSO_0_9_24:=false
USE_BUSYBOX_SNAPSHOT:=$(SNAPSHOT)
USE_NETFILTER_SNAPSHOT:=$(SNAPSHOT)
USE_BRIDGE_VERSION:=0.9.7
BUILD_WITH_LARGEFILE:=true

OPTIMIZE_FOR_CPU=$(ARCH)

# Command used to download source code
WGET:=wget --passive-ftp

# Soft floating point options.
# Notes:
#   Currently builds with gcc 3.3 for arm, mips, mipsel, powerpc.
#   (i386 support will be added back in at some point.)
#   Only tested with multilib enabled.
#   For i386, long double is the same as double (64 bits).  While this
#      is unusual for x86, it seemed the best approach considering the
#      limitations in the gcc floating point emulation library.
#   For arm, soft float uses the usual libfloat routines.
#   Custom specs files are used to set the default gcc mode to soft float
#      as a convenience, since you shouldn't link hard and soft float
#      together.  In fact, arm won't even let you.
# (Un)comment the appropriate line below.
#SOFT_FLOAT:=true
SOFT_FLOAT:=false

TARGET_OPTIMIZATION=-Os -mips2
TARGET_DEBUGGING= #-g

# Currently the unwind stuff seems to work for staticly linked apps but
# not dynamic.  So use setjmp/longjmp exceptions by default.
# GCC_USE_SJLJ_EXCEPTIONS:=--enable-sjlj-exceptions
GCC_USE_SJLJ_EXCEPTIONS:=

# Any additional gcc options you may want to include....
EXTRA_GCC_CONFIG_OPTIONS:=

# Enable the following if you want locale/gettext/i18n support.
# NOTE!  Currently the pregnerated locale stuff only works for x86!
#ENABLE_LOCALE:=true
ENABLE_LOCALE:=false

# If you want multilib enabled, enable this...
MULTILIB:=--enable-multilib

# Build/install c++ compiler and libstdc++?
#INSTALL_LIBSTDCPP:=true
INSTALL_LIBSTDCPP:=false

# Build/install java compiler and libgcj? (requires c++)
# WARNING!!! DOES NOT BUILD FOR TARGET WITHOUT INTERVENTION!!!  mjn3
#INSTALL_LIBGCJ:=true
INSTALL_LIBGCJ:=false

# For SMP machines some stuff can be run in parallel
#JLEVEL=-j3

#############################################################
#
# The list of stuff to build for the target filesystem
#
#############################################################
TARGETS:=gcc3_3 openwrt-base openwrt-code.bin

#############################################################
#
# You should probably leave this stuff alone unless you know
# what you are doing.
#
#############################################################

ifeq ($(SOFT_FLOAT),true)
SOFT_FLOAT_CONFIG_OPTION:=--without-float
TARGET_SOFT_FLOAT:=-msoft-float
ARCH_FPU_SUFFIX:=_nofpu
else
SOFT_FLOAT_CONFIG_OPTION:=
TARGET_SOFT_FLOAT:=
ARCH_FPU_SUFFIX:=
endif

ifeq ($(INSTALL_LIBGCJ),true)
INSTALL_LIBSTDCPP:=true
endif

# WARNING -- uClibc currently disables large file support on cris.
ifeq ("$(strip $(ARCH))","cris")
BUILD_WITH_LARGEFILE:=false
endif

ifneq ($(BUILD_WITH_LARGEFILE),true)
DISABLE_LARGEFILE= --disable-largefile
endif
TARGET_CFLAGS=$(TARGET_OPTIMIZATION) $(TARGET_DEBUGGING)

HOSTCC:=gcc
BASE_DIR:=${shell pwd}
SOURCE_DIR:=$(BASE_DIR)/sources
DL_DIR:=$(SOURCE_DIR)/dl
PATCH_DIR=$(SOURCE_DIR)/patches
BUILD_DIR:=$(BASE_DIR)/build_$(ARCH)$(ARCH_FPU_SUFFIX)
TARGET_DIR:=$(BUILD_DIR)/root
STAGING_DIR=$(BUILD_DIR)/staging_dir
TOOL_BUILD_DIR=$(BASE_DIR)/toolchain_build_$(ARCH)$(ARCH_FPU_SUFFIX)
TARGET_PATH=$(STAGING_DIR)/bin:/bin:/sbin:/usr/bin:/usr/sbin
IMAGE:=$(BASE_DIR)/root_fs_$(ARCH)$(ARCH_FPU_SUFFIX)
REAL_GNU_TARGET_NAME=$(OPTIMIZE_FOR_CPU)-linux-uclibc
GNU_TARGET_NAME=$(OPTIMIZE_FOR_CPU)-linux
KERNEL_CROSS=$(STAGING_DIR)/bin/$(OPTIMIZE_FOR_CPU)-linux-uclibc-
TARGET_CROSS=$(STAGING_DIR)/bin/$(OPTIMIZE_FOR_CPU)-linux-uclibc-
TARGET_CC=$(TARGET_CROSS)gcc
STRIP=$(TARGET_CROSS)strip --remove-section=.comment --remove-section=.note


HOST_ARCH:=$(shell $(HOSTCC) -dumpmachine | sed -e s'/-.*//' \
	-e 's/sparc.*/sparc/' \
	-e 's/arm.*/arm/g' \
	-e 's/m68k.*/m68k/' \
	-e 's/ppc/powerpc/g' \
	-e 's/v850.*/v850/g' \
	-e 's/sh[234]/sh/' \
	-e 's/mips-.*/mips/' \
	-e 's/mipsel-.*/mipsel/' \
	-e 's/cris.*/cris/' \
	-e 's/i[3-9]86/i386/' \
	)
GNU_HOST_NAME:=$(HOST_ARCH)-pc-linux-gnu
TARGET_CONFIGURE_OPTS=PATH=$(TARGET_PATH) \
		AR=$(TARGET_CROSS)ar \
		AS=$(TARGET_CROSS)as \
		LD=$(TARGET_CROSS)ld \
		NM=$(TARGET_CROSS)nm \
		CC=$(TARGET_CROSS)gcc \
		GCC=$(TARGET_CROSS)gcc \
		CXX=$(TARGET_CROSS)g++ \
		RANLIB=$(TARGET_CROSS)ranlib

ifeq ($(ENABLE_LOCALE),true)
DISABLE_NLS:=
else
DISABLE_NLS:=--disable-nls
endif


all:   world

TARGETS_CLEAN:=$(patsubst %,%-clean,$(TARGETS))
TARGETS_SOURCE:=$(patsubst %,%-source,$(TARGETS))
TARGETS_DIRCLEAN:=$(patsubst %,%-dirclean,$(TARGETS))

world: $(DL_DIR) $(BUILD_DIR) $(STAGING_DIR) $(TARGET_DIR) $(TARGETS)
	@echo "ALL DONE."

.PHONY: all world clean dirclean distclean source $(TARGETS) \
	$(TARGETS_CLEAN) $(TARGETS_DIRCLEAN) $(TARGETS_SOURCE)

include make/*.mk

#############################################################
#
# staging and target directories do NOT list these as
# dependancies anywhere else
#
#############################################################
$(DL_DIR):
	mkdir $(DL_DIR)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(STAGING_DIR):
	rm -rf $(STAGING_DIR)
	mkdir -p $(STAGING_DIR)/lib
	mkdir -p $(STAGING_DIR)/usr
	ln -fs $(REAL_GNU_TARGET_NAME)/include $(STAGING_DIR)/include
	ln -fs ../lib $(STAGING_DIR)/usr/lib
	ln -fs ../$(REAL_GNU_TARGET_NAME)/include $(STAGING_DIR)/usr/include


$(TARGET_DIR): $(DL_DIR)/$(OPENWRT_ROOT_SKEL)
	rm -rf $(TARGET_DIR)
	#zcat $(SOURCE_DIR)/skel.tar.gz | tar -C $(BUILD_DIR) -xf -
	zcat $(DL_DIR)/$(OPENWRT_ROOT_SKEL) | tar -C $(BUILD_DIR) -xf -
	#cp -a $(SOURCE_DIR)/target_skeleton/* $(TARGET_DIR)/
	-find $(TARGET_DIR) -type d -name CVS -exec rm -rf {} \; > /dev/null 2>&1

source: $(TARGETS_SOURCE)

#############################################################
#
# Cleanup and misc junk
#
#############################################################
clean:
	rm -rf $(TOOL_BUILD_DIR) $(BUILD_DIR) $(IMAGE) \
		openwrt-linux.trx openwrt-g-code.bin openwrt-gs-code.bin \
		openwrt-kmodules.tar.bz2

dirclean: $(TARGETS_DIRCLEAN)
	rm -rf $(TARGET_DIR) $(STAGING_DIR) $(IMAGE) \
		openwrt-linux.trx openwrt-g-code.bin openwrt-gs-code.bin \
		openwrt-kmodules.tar.bz2

distclean:
	rm -rf $(DL_DIR) $(BUILD_DIR) $(LINUX_KERNEL) $(IMAGE) \
		openwrt-linux.trx openwrt-g-code.bin openwrt-gs-code.bin \
		openwrt-kmodules.tar.bz2

sourceball: 
	rm -rf $(BUILD_DIR)
	set -e; \
	cd ..; \
	rm -f buildroot.tar.bz2; \
	tar -cvf buildroot.tar buildroot; \
	bzip2 -9 buildroot.tar; \

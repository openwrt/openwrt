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

# What sortof target system shall we compile this for?
#ARCH:=i386
#ARCH:=arm
#ARCH:=mips
ARCH:=mipsel
#ARCH:=powerpc
#ARCH:=sh4
# Busybox link failing due to needing libgcc functions that are statics.
#ARCH:=cris

# The following currently fail to build since no shared lib support.
#ARCH:=sh64
#ARCH:=m68k
#ARCH:=v850
#ARCH:=sparc
#ARCH:=whatever

# If you are building a native gcc toolchain, do you want to
# build the old gcc-2.95 based toolchain, or would you prefer
# a nice and shiny new gcc-3.3.2 toolchain?
# WARNING -- 2.95 currently only builds for i386, arm, mips*, and powerpc.
# WARNING -- 2.95 does not currently build natively for the target.
GCC_2_95_TOOLCHAIN:=false

# Enable this to use the uClibc daily snapshot instead of a released
# version.  Daily snapshots may contain new features and bugfixes. Or
# they may not even compile at all, depending on what Erik is doing...
#USE_UCLIBC_SNAPSHOT:=
#USE_UCLIBC_SNAPSHOT:=snapshot
USE_UCLIBC_SNAPSHOT:=20040509

# Temporary option... Fall back to (slightly modified) ldso 0.9.24 code?
# This is here temporarily since I'm having problems with the current
# cvs ldso code on mipsel.
USE_UCLIBC_LDSO_0_9_24:=true
#USE_UCLIBC_LDSO_0_9_24:=false

# Enable this to use the busybox daily snapshot instead of a released
# version.  Daily snapshots may contain new features and bugfixes. Or
# they may not even compile at all....
#USE_BUSYBOX_SNAPSHOT:=
#USE_BUSYBOX_SNAPSHOT:=snapshot
USE_BUSYBOX_SNAPSHOT:=20040509

# Enable large file (files > 2 GB) support
BUILD_WITH_LARGEFILE:=true

# Command used to download source code
WGET:=wget --passive-ftp

# Optimize toolchain for which type of CPU?
OPTIMIZE_FOR_CPU=$(ARCH)
#OPTIMIZE_FOR_CPU=i686
# Note... gcc 2.95 does not seem to like anything higher than i586.
#OPTIMIZE_FOR_CPU=i586
#OPTIMIZE_FOR_CPU=whatever

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
GCC_USE_SJLJ_EXCEPTIONS:=--enable-sjlj-exceptions
#GCC_USE_SJLJ_EXCEPTIONS:=

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
TARGETS:=host-sed

ifeq ($(GCC_2_95_TOOLCHAIN),true)
TARGETS+=uclibc-configured binutils gcc2_95
else
TARGETS+=uclibc-configured binutils gcc3_3
endif

# Do you want user mode Linux (x86 only), or are you building a
# your own kernel that will run on its own?  Perhaps you have a
# kernel you have already configured and you want to use that?
# The default is to just use a set of known working kernel
# headers.  Unless you want to build a kernel, I recommend just
# using that...
#TARGETS+=kernel-headers
#TARGETS+=linux
#TARGETS+=user-mode-linux
#TARGETS+=system-linux
TARGETS+= openwrt-base

# The default minimal set
#TARGETS+=busybox tinylogin

# Openssh...
#TARGETS+=zlib openssl openssh
# Dropbear sshd is much smaller than openssl + openssh
#TARGETS+=dropbear_sshd

# Everything needed to build a full uClibc development system!
#TARGETS+=coreutils findutils bash make diffutils patch sed
#TARGETS+=ed flex bison file gawk tar grep bzip2

#If you want a development system, you probably want gcc built
# with uClibc so it can run within your dev system...
#TARGETS+=gcc2_95_target ccache_target   # NOT WORKING!!!
#TARGETS+=gcc3_3_target ccache_target

# Of course, if you are installing a development system, you
# may want some header files so you can compile stuff....
#TARGETS+=ncurses-headers zlib-headers openssl-headers

# More development system stuff for those that want it
#TARGETS+=m4 autoconf automake libtool

# Perl
#TARGETS+=perl

# Some nice debugging tools
#TARGETS+=gdb strace ltrace

# The Valgrind debugger (x86 only)
#TARGETS+=valgrind

# Some stuff for access points and firewalls
#TARGETS+=iptables hostap wtools dhcp_relay bridge
#TARGETS+=iproute2 netsnmp

# Run customize.mk at the very end to add your own special config.
# This is useful for making your own distro within the buildroot
# process.
# TARGETS+=customize

#############################################################
#
# Pick your root filesystem type.
#
#############################################################
#TARGETS+=ext2root

# Must mount cramfs with 'ramdisk_blocksize=4096'
#TARGETS+=cramfsroot

# You may need to edit make/jffs2root.mk to change target
# endian-ness or similar, but this is sufficient for most
# things as-is...
#TARGETS+=jffs2root

#TARGETS+= openwrt-linux.trx
TARGETS+= openwrt-code.bin

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

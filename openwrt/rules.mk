# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

-include $(TOPDIR)/.config
include $(TOPDIR)/include/verbose.mk

export SHELL=/bin/bash

ARCH:=$(strip $(subst ",, $(CONFIG_ARCH)))
TARGET_OPTIMIZATION:=$(strip $(subst ",, $(CONFIG_TARGET_OPTIMIZATION)))
WGET:=$(strip $(subst ",, $(CONFIG_WGET)))
#"))"))")) # fix vim's broken syntax highlighting

OPTIMIZE_FOR_CPU:=$(ARCH)

# DIRECTORIES #

DL_DIR:=$(TOPDIR)/dl
INCLUDE_DIR:=$(TOPDIR)/include
SCRIPT_DIR:=$(TOPDIR)/scripts
TOOL_BUILD_DIR:=$(TOPDIR)/toolchain_build_$(ARCH)
STAGING_DIR:=$(TOPDIR)/staging_dir_$(ARCH)
BIN_DIR:=$(TOPDIR)/bin
PACKAGE_DIR:=$(BIN_DIR)/packages
IPKG_TARGET_DIR:=$(PACKAGE_DIR)
BUILD_DIR:=$(TOPDIR)/build_$(ARCH)
STAMP_DIR:=$(BUILD_DIR)/stamp
TARGET_DIR:=$(BUILD_DIR)/root
IPKG_STATE_DIR:=$(TARGET_DIR)/usr/lib/ipkg

REAL_GNU_TARGET_NAME=$(OPTIMIZE_FOR_CPU)-linux-uclibc
GNU_TARGET_NAME=$(OPTIMIZE_FOR_CPU)-linux
KERNEL_CROSS:=$(STAGING_DIR)/bin/$(OPTIMIZE_FOR_CPU)-linux-uclibc-
TARGET_CROSS:=$(STAGING_DIR)/bin/$(OPTIMIZE_FOR_CPU)-linux-uclibc-
IMAGE:=$(BUILD_DIR)/root_fs_$(ARCH)

TARGET_PATH:=$(STAGING_DIR)/usr/bin:$(STAGING_DIR)/bin:/bin:/sbin:/usr/bin:/usr/sbin
TARGET_CFLAGS:=$(TARGET_OPTIMIZATION)

LINUX_DIR:=$(BUILD_DIR)/linux
LINUX_HEADERS_DIR:=$(TOOL_BUILD_DIR)/linux

# APPLICATIONS #
HOSTCC:=gcc
TARGET_CC:=$(TARGET_CROSS)gcc
STRIP:=$(STAGING_DIR)/bin/sstrip
PATCH:=$(SCRIPT_DIR)/patch-kernel.sh
SED:=$(STAGING_DIR)/bin/sed -i -e
CP:=cp -fpR

HOST_ARCH:=$(shell $(HOSTCC) -dumpmachine | sed -e s'/-.*//' \
	-e 's/sparc.*/sparc/' \
	-e 's/arm.*/arm/' \
	-e 's/m68k.*/m68k/' \
	-e 's/ppc/powerpc/' \
	-e 's/v850.*/v850/' \
	-e 's/sh[234]/sh/' \
	-e 's/mips-.*/mips/' \
	-e 's/mipsel-.*/mipsel/' \
	-e 's/cris.*/cris/' \
	-e 's/i[3-9]86/i386/' \
	)

GNU_HOST_NAME:=$(HOST_ARCH)-pc-linux-gnu

TARGET_CONFIGURE_OPTS:= \
  PATH=$(TARGET_PATH) \
  AR=$(TARGET_CROSS)ar \
  AS=$(TARGET_CROSS)as \
  LD=$(TARGET_CROSS)ld \
  NM=$(TARGET_CROSS)nm \
  CC=$(TARGET_CROSS)gcc \
  GCC=$(TARGET_CROSS)gcc \
  CXX=$(TARGET_CROSS)g++ \
  RANLIB=$(TARGET_CROSS)ranlib \
  STRIP=$(TARGET_CROSS)strip

# strip an entire directory
RSTRIP:= \
  STRIP="$(STRIP)" \
  STRIP_KMOD="$(TARGET_CROSS)strip --strip-unneeded --remove-section=.comment" \
  $(SCRIPT_DIR)/rstrip.sh

# where to build (and put) .ipk packages
IPKG:= \
  IPKG_TMP=$(BUILD_DIR)/tmp \
  IPKG_INSTROOT=$(TARGET_DIR) \
  IPKG_CONF_DIR=$(STAGING_DIR)/etc \
  IPKG_OFFLINE_ROOT=$(BUILD_DIR)/root \
  $(SCRIPT_DIR)/ipkg -force-defaults -force-depends

# invoke ipkg-build with some default options
IPKG_BUILD:= \
  PATH="$(TARGET_PATH)" ipkg-build -c -o root -g root

ifeq ($(CONFIG_ENABLE_LOCALE),true)
  DISABLE_NLS:=
else
  DISABLE_NLS:=--disable-nls
endif

ifneq ($(CONFIG_LARGEFILE),y)
  DISABLE_LARGEFILE= --disable-largefile
endif

ifeq ($(CONFIG_TAR_VERBOSITY),y)
  TAR_OPTIONS=-xvf
else
  TAR_OPTIONS=-xf
endif

all:
FORCE: ;
.PHONY: FORCE

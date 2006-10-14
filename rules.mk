# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

-include $(TOPDIR)/.config
include $(TOPDIR)/include/verbose.mk

export SHELL=/usr/bin/env bash -c '. $(TOPDIR)/include/shell.sh; eval "$$2"' --
export BASH=$(shell which bash)

ARCH:=$(strip $(subst ",, $(CONFIG_ARCH)))
TARGET_OPTIMIZATION:=$(strip $(subst ",, $(CONFIG_TARGET_OPTIMIZATION)))
WGET:=$(strip $(subst ",, $(CONFIG_WGET)))
#"))"))")) # fix vim's broken syntax highlighting

OPTIMIZE_FOR_CPU:=$(ARCH)

# DIRECTORIES #

DL_DIR:=$(TOPDIR)/dl
INCLUDE_DIR:=$(TOPDIR)/include
SCRIPT_DIR:=$(TOPDIR)/scripts
TOOL_BUILD_DIR:=$(TOPDIR)/tool_build
TOOLCHAIN_BUILD_DIR:=$(TOPDIR)/toolchain_build_$(ARCH)
STAGING_DIR:=$(TOPDIR)/staging_dir_$(ARCH)
BIN_DIR:=$(TOPDIR)/bin
PACKAGE_DIR:=$(BIN_DIR)/packages
IPKG_TARGET_DIR:=$(PACKAGE_DIR)
BUILD_DIR:=$(TOPDIR)/build_$(ARCH)
TMP_DIR:=$(BUILD_DIR)/tmp
STAMP_DIR:=$(BUILD_DIR)/stamp
TARGET_DIR:=$(BUILD_DIR)/root
IPKG_STATE_DIR:=$(TARGET_DIR)/usr/lib/ipkg

ifeq ($(CONFIG_NATIVE_TOOLCHAIN),)
  REAL_GNU_TARGET_NAME=$(OPTIMIZE_FOR_CPU)-linux-uclibc
  GNU_TARGET_NAME=$(OPTIMIZE_FOR_CPU)-linux
  TARGET_CROSS:=$(OPTIMIZE_FOR_CPU)-linux-uclibc-
endif

IMAGE:=$(BUILD_DIR)/root_fs_$(ARCH)

TARGET_PATH:=$(STAGING_DIR)/usr/bin:$(STAGING_DIR)/bin:$(PATH)
TARGET_CFLAGS:=$(TARGET_OPTIMIZATION)

export PATH:=$(TARGET_PATH)

LINUX_DIR:=$(BUILD_DIR)/linux
LINUX_HEADERS_DIR:=$(TOOLCHAIN_BUILD_DIR)/linux

# APPLICATIONS #
HOSTCC:=gcc
TARGET_CC:=$(TARGET_CROSS)gcc
STRIP:=$(STAGING_DIR)/bin/sstrip
PATCH:=$(SCRIPT_DIR)/patch-kernel.sh
SED:=$(STAGING_DIR)/bin/sed -i -e
CP:=cp -fpR

ifneq ($(CONFIG_CCACHE),)
  # FIXME: move this variable to a better location
  export CCACHE_DIR=$(STAGING_DIR)/ccache
  TARGET_CC:= ccache $(TARGET_CC)
endif

TARGET_CONFIGURE_OPTS:= \
  AR=$(TARGET_CROSS)ar \
  AS="$(TARGET_CC) -c $(TARGET_CFLAGS)" \
  LD=$(TARGET_CROSS)ld \
  NM=$(TARGET_CROSS)nm \
  CC="$(TARGET_CC)" \
  GCC="$(TARGET_CC)" \
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
  PATH="$(STAGING_DIR)/bin:$(PATH)" \
  IPKG_TMP=$(BUILD_DIR)/tmp \
  IPKG_INSTROOT=$(TARGET_DIR) \
  IPKG_CONF_DIR=$(STAGING_DIR)/etc \
  IPKG_OFFLINE_ROOT=$(BUILD_DIR)/root \
  $(SCRIPT_DIR)/ipkg -force-defaults -force-depends

# invoke ipkg-build with some default options
IPKG_BUILD:= \
  ipkg-build -c -o 0 -g 0

ifeq ($(CONFIG_ENABLE_LOCALE),true)
  DISABLE_NLS:=
else
  DISABLE_NLS:=--disable-nls
endif

ifneq ($(CONFIG_LARGEFILE),y)
  DISABLE_LARGEFILE= --disable-largefile
endif

ifeq ($(CONFIG_TAR_VERBOSITY),y)
  TAR_OPTIONS:=-xvf -
else
  TAR_OPTIONS:=-xf -
endif

all:
FORCE: ;
.PHONY: FORCE
%: ;

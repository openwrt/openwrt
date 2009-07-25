#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

ifneq ($(__rules_inc),1)
__rules_inc=1

ifeq ($(DUMP),)
  -include $(TOPDIR)/.config
endif
include $(TOPDIR)/include/debug.mk
include $(TOPDIR)/include/verbose.mk

TMP_DIR:=$(TOPDIR)/tmp

export SHELL=/usr/bin/env bash -c '. $(TOPDIR)/include/shell.sh; eval "$$2"' --

qstrip=$(strip $(subst ",,$(1)))
#"))

empty:=
space:= $(empty) $(empty)
merge=$(subst $(space),,$(1))
confvar=$(call merge,$(foreach v,$(1),$(if $($(v)),y,n)))
strip_last=$(patsubst %.$(lastword $(subst .,$(space),$(1))),%,$(1))

_SINGLE=export MAKEFLAGS=$(space);
CFLAGS:=
ARCH:=$(subst i486,i386,$(subst i586,i386,$(subst i686,i386,$(call qstrip,$(CONFIG_ARCH)))))
BOARD:=$(call qstrip,$(CONFIG_TARGET_BOARD))
TARGET_OPTIMIZATION:=$(call qstrip,$(CONFIG_TARGET_OPTIMIZATION))
TARGET_SUFFIX=$(call qstrip,$(CONFIG_TARGET_SUFFIX))
BUILD_SUFFIX:=$(call qstrip,$(CONFIG_BUILD_SUFFIX))
GCCV:=$(call qstrip,$(CONFIG_GCC_VERSION))
LIBC:=$(call qstrip,$(CONFIG_LIBC))
LIBCV:=$(call qstrip,$(CONFIG_LIBC_VERSION))
SUBDIR:=$(patsubst $(TOPDIR)/%,%,${CURDIR})

OPTIMIZE_FOR_CPU=$(subst i386,i486,$(ARCH))

ifeq ($(ARCH),powerpc)
  FPIC:=-fPIC
else
  FPIC:=-fpic
endif

DL_DIR:=$(if $(call qstrip,$(CONFIG_DOWNLOAD_FOLDER)),$(call qstrip,$(CONFIG_DOWNLOAD_FOLDER)),$(TOPDIR)/dl)
BIN_DIR:=$(TOPDIR)/bin
INCLUDE_DIR:=$(TOPDIR)/include
SCRIPT_DIR:=$(TOPDIR)/scripts
BUILD_DIR_BASE:=$(TOPDIR)/build_dir
BUILD_DIR:=$(BUILD_DIR_BASE)/target-$(ARCH)_$(LIBC)-$(LIBCV)$(if $(BUILD_SUFFIX),_$(BUILD_SUFFIX))
BUILD_DIR_HOST:=$(BUILD_DIR_BASE)/host
BUILD_DIR_TOOLCHAIN:=$(BUILD_DIR_BASE)/toolchain-$(ARCH)_gcc-$(GCCV)_$(LIBC)-$(LIBCV)
STAGING_DIR:=$(TOPDIR)/staging_dir/target-$(ARCH)_$(LIBC)-$(LIBCV)
STAGING_DIR_HOST:=$(TOPDIR)/staging_dir/host
TOOLCHAIN_DIR:=$(TOPDIR)/staging_dir/toolchain-$(ARCH)_gcc-$(GCCV)_$(LIBC)-$(LIBCV)
PACKAGE_DIR:=$(BIN_DIR)/packages/target-$(ARCH)_$(LIBC)-$(LIBCV)
STAMP_DIR:=$(BUILD_DIR)/stamp
STAMP_DIR_HOST=$(BUILD_DIR_HOST)/stamp
TARGET_ROOTFS_DIR?=$(if $(call qstrip,$(CONFIG_TARGET_ROOTFS_DIR)),$(call qstrip,$(CONFIG_TARGET_ROOTFS_DIR)),$(BUILD_DIR))
TARGET_DIR:=$(TARGET_ROOTFS_DIR)/root-$(BOARD)
DEBUG_DIR:=$(BUILD_DIR)/debug-$(BOARD)
BUILD_LOG_DIR:=$(TOPDIR)/logs

TARGET_PATH:=$(TOOLCHAIN_DIR)/usr/bin:$(STAGING_DIR_HOST)/bin:$(PATH)
TARGET_PATH_PKG:=$(STAGING_DIR)/host/bin:$(TARGET_PATH)
TARGET_CFLAGS:=$(TARGET_OPTIMIZATION)$(if $(CONFIG_DEBUG), -g3)
TARGET_CPPFLAGS:=-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include
TARGET_LDFLAGS:=-L$(TOOLCHAIN_DIR)/usr/lib -L$(TOOLCHAIN_DIR)/lib -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib
LIBGCC_S=$(if $(wildcard $(TOOLCHAIN_DIR)/lib/libgcc_s.so),-lgcc_s,$(wildcard $(TOOLCHAIN_DIR)/lib/gcc/*/*/libgcc.a))

ifndef DUMP
ifeq ($(CONFIG_NATIVE_TOOLCHAIN),)
  -include $(TOOLCHAIN_DIR)/info.mk
  REAL_GNU_TARGET_NAME=$(OPTIMIZE_FOR_CPU)-openwrt-linux$(if $(TARGET_SUFFIX),-$(TARGET_SUFFIX))
  GNU_TARGET_NAME=$(OPTIMIZE_FOR_CPU)-openwrt-linux
  TARGET_CROSS:=$(if $(TARGET_CROSS),$(TARGET_CROSS),$(OPTIMIZE_FOR_CPU)-openwrt-linux$(if $(TARGET_SUFFIX),-$(TARGET_SUFFIX))-)
  TARGET_CFLAGS+= -fhonour-copts
endif
endif

ifeq ($(CONFIG_SOFT_FLOAT),y)
  SOFT_FLOAT_CONFIG_OPTION:=--with-float=soft
  TARGET_CFLAGS+= -msoft-float
else
  SOFT_FLOAT_CONFIG_OPTION:=
endif

export PATH:=$(TARGET_PATH)
export STAGING_DIR
export GCC_HONOUR_COPTS:=0

PKG_CONFIG:=$(STAGING_DIR_HOST)/bin/pkg-config

export PKG_CONFIG

HOSTCC:=gcc
HOST_CFLAGS:=-O2 -I$(STAGING_DIR_HOST)/include
HOST_LDFLAGS:=-L$(STAGING_DIR_HOST)/lib

TARGET_CC:=$(TARGET_CROSS)gcc
TARGET_CXX:=$(if $(CONFIG_INSTALL_LIBSTDCPP),$(TARGET_CROSS)g++,no)
PATCH:=$(SCRIPT_DIR)/patch-kernel.sh
SED:=$(STAGING_DIR_HOST)/bin/sed -i -e
CP:=cp -fpR

INSTALL_BIN:=install -m0755
INSTALL_DIR:=install -d -m0755
INSTALL_DATA:=install -m0644
INSTALL_CONF:=install -m0600

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
  CXX="$(TARGET_CXX)" \
  RANLIB=$(TARGET_CROSS)ranlib \
  STRIP=$(TARGET_CROSS)strip \
  OBJCOPY=$(TARGET_CROSS)objcopy \
  OBJDUMP=$(TARGET_CROSS)objdump \
  SIZE=$(TARGET_CROSS)size

# strip an entire directory
ifneq ($(CONFIG_NO_STRIP),)
  RSTRIP:=:
  STRIP:=:
else
  ifneq ($(CONFIG_USE_STRIP),)
    STRIP:=$(TARGET_CROSS)strip
  else
    ifneq ($(CONFIG_USE_SSTRIP),)
      STRIP:=$(STAGING_DIR_HOST)/bin/sstrip
    endif
  endif
  RSTRIP:= \
    NM="$(TARGET_CROSS)nm" \
    STRIP="$(STRIP)" \
    STRIP_KMOD="$(TARGET_CROSS)strip --strip-unneeded --remove-section=.comment" \
    $(SCRIPT_DIR)/rstrip.sh
endif

ifeq ($(CONFIG_ENABLE_LOCALE),true)
  DISABLE_NLS:=--enable-nls
else
  DISABLE_NLS:=--disable-nls
endif

ifeq ($(CONFIG_IPV6),y)
  DISABLE_IPV6:=--enable-ipv6
else
  DISABLE_IPV6:=--disable-ipv6
endif

ifeq ($(CONFIG_LARGEFILE),y)
  DISABLE_LARGEFILE:=
else
  DISABLE_LARGEFILE:=--disable-largefile
endif

ifeq ($(CONFIG_TAR_VERBOSITY),y)
  TAR_OPTIONS:=-xvf -
else
  TAR_OPTIONS:=-xf -
endif

define shvar
V_$(subst .,_,$(subst -,_,$(subst /,_,$(1))))
endef

define shexport
$(call shvar,$(1))=$$(call $(1))
export $(call shvar,$(1))
endef

# file extension
ext=$(word $(words $(subst ., ,$(1))),$(subst ., ,$(1)))

all:
FORCE: ;
.PHONY: FORCE

endif #__rules_inc

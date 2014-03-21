#
# Copyright (C) 2006-2010 OpenWrt.org
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

export TMP_DIR:=$(TOPDIR)/tmp

GREP_OPTIONS=
export GREP_OPTIONS

qstrip=$(strip $(subst ",,$(1)))
#"))

empty:=
space:= $(empty) $(empty)
merge=$(subst $(space),,$(1))
confvar=$(call merge,$(foreach v,$(1),$(if $($(v)),y,n)))
strip_last=$(patsubst %.$(lastword $(subst .,$(space),$(1))),%,$(1))

define sep

endef

_SINGLE=export MAKEFLAGS=$(space);
CFLAGS:=
ARCH:=$(subst i486,i386,$(subst i586,i386,$(subst i686,i386,$(call qstrip,$(CONFIG_ARCH)))))
ARCH_PACKAGES:=$(call qstrip,$(CONFIG_TARGET_ARCH_PACKAGES))
BOARD:=$(call qstrip,$(CONFIG_TARGET_BOARD))
TARGET_OPTIMIZATION:=$(call qstrip,$(CONFIG_TARGET_OPTIMIZATION))
export EXTRA_OPTIMIZATION:=$(call qstrip,$(CONFIG_EXTRA_OPTIMIZATION))
TARGET_SUFFIX=$(call qstrip,$(CONFIG_TARGET_SUFFIX))
BUILD_SUFFIX:=$(call qstrip,$(CONFIG_BUILD_SUFFIX))
SUBDIR:=$(patsubst $(TOPDIR)/%,%,${CURDIR})
export SHELL:=/usr/bin/env bash

OPTIMIZE_FOR_CPU=$(subst i386,i486,$(ARCH))

ifeq ($(ARCH),powerpc)
  FPIC:=-fPIC
else
  FPIC:=-fpic
endif

HOST_FPIC:=-fPIC

ARCH_SUFFIX:=$(call qstrip,$(CONFIG_CPU_TYPE))
GCC_ARCH:=

ifneq ($(ARCH_SUFFIX),)
  ARCH_SUFFIX:=_$(ARCH_SUFFIX)
endif
ifneq ($(filter -march=armv%,$(TARGET_OPTIMIZATION)),)
  GCC_ARCH:=$(patsubst -march=%,%,$(filter -march=armv%,$(TARGET_OPTIMIZATION)))
endif
ifdef CONFIG_HAS_SPE_FPU
  TARGET_SUFFIX:=$(TARGET_SUFFIX)spe
endif
ifdef CONFIG_MIPS64_ABI
  ifneq ($(CONFIG_MIPS64_ABI_O32),y)
     ARCH_SUFFIX:=$(ARCH_SUFFIX)_$(call qstrip,$(CONFIG_MIPS64_ABI))
  endif
endif

DL_DIR:=$(if $(call qstrip,$(CONFIG_DOWNLOAD_FOLDER)),$(call qstrip,$(CONFIG_DOWNLOAD_FOLDER)),$(TOPDIR)/dl)
BIN_DIR:=$(if $(call qstrip,$(CONFIG_BINARY_FOLDER)),$(call qstrip,$(CONFIG_BINARY_FOLDER)),$(TOPDIR)/bin/$(BOARD))
INCLUDE_DIR:=$(TOPDIR)/include
SCRIPT_DIR:=$(TOPDIR)/scripts
BUILD_DIR_BASE:=$(TOPDIR)/build_dir
BUILD_DIR_HOST:=$(BUILD_DIR_BASE)/host
STAGING_DIR_HOST:=$(TOPDIR)/staging_dir/host
ifeq ($(CONFIG_EXTERNAL_TOOLCHAIN),)
  GCCV:=$(call qstrip,$(CONFIG_GCC_VERSION))
  LIBC:=$(call qstrip,$(CONFIG_LIBC))
  LIBCV:=$(call qstrip,$(CONFIG_LIBC_VERSION))
  REAL_GNU_TARGET_NAME=$(OPTIMIZE_FOR_CPU)-openwrt-linux$(if $(TARGET_SUFFIX),-$(TARGET_SUFFIX))
  GNU_TARGET_NAME=$(OPTIMIZE_FOR_CPU)-openwrt-linux
  DIR_SUFFIX:=_$(LIBC)-$(LIBCV)$(if $(CONFIG_arm),_eabi)
  BIN_DIR:=$(BIN_DIR)$(if $(CONFIG_USE_UCLIBC),,-$(LIBC))
  TARGET_DIR_NAME = target-$(ARCH)$(ARCH_SUFFIX)$(DIR_SUFFIX)$(if $(BUILD_SUFFIX),_$(BUILD_SUFFIX))
  TOOLCHAIN_DIR_NAME = toolchain-$(ARCH)$(ARCH_SUFFIX)_gcc-$(GCCV)$(DIR_SUFFIX)
else
  ifeq ($(CONFIG_NATIVE_TOOLCHAIN),)
    GNU_TARGET_NAME=$(call qstrip,$(CONFIG_TARGET_NAME))
  else
    GNU_TARGET_NAME=$(shell gcc -dumpmachine)
  endif
  REAL_GNU_TARGET_NAME=$(GNU_TARGET_NAME)
  TARGET_DIR_NAME:=target-$(GNU_TARGET_NAME)$(if $(BUILD_SUFFIX),_$(BUILD_SUFFIX))
  TOOLCHAIN_DIR_NAME:=toolchain-$(GNU_TARGET_NAME)
endif

PACKAGE_DIR:=$(BIN_DIR)/packages
BUILD_DIR:=$(BUILD_DIR_BASE)/$(TARGET_DIR_NAME)
STAGING_DIR:=$(TOPDIR)/staging_dir/$(TARGET_DIR_NAME)
BUILD_DIR_TOOLCHAIN:=$(BUILD_DIR_BASE)/$(TOOLCHAIN_DIR_NAME)
TOOLCHAIN_DIR:=$(TOPDIR)/staging_dir/$(TOOLCHAIN_DIR_NAME)
STAMP_DIR:=$(BUILD_DIR)/stamp
STAMP_DIR_HOST=$(BUILD_DIR_HOST)/stamp
TARGET_ROOTFS_DIR?=$(if $(call qstrip,$(CONFIG_TARGET_ROOTFS_DIR)),$(call qstrip,$(CONFIG_TARGET_ROOTFS_DIR)),$(BUILD_DIR))
TARGET_DIR:=$(TARGET_ROOTFS_DIR)/root-$(BOARD)
STAGING_DIR_ROOT:=$(STAGING_DIR)/root-$(BOARD)
BUILD_LOG_DIR:=$(TOPDIR)/logs
PKG_INFO_DIR := $(STAGING_DIR)/pkginfo

TARGET_PATH:=$(STAGING_DIR_HOST)/bin:$(subst $(space),:,$(filter-out .,$(filter-out ./,$(subst :,$(space),$(PATH)))))
TARGET_CFLAGS:=$(TARGET_OPTIMIZATION)$(if $(CONFIG_DEBUG), -g3) $(EXTRA_OPTIMIZATION)
TARGET_CXXFLAGS = $(TARGET_CFLAGS)
TARGET_ASFLAGS_DEFAULT = $(TARGET_CFLAGS)
TARGET_ASFLAGS = $(TARGET_ASFLAGS_DEFAULT)
TARGET_CPPFLAGS:=-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include
TARGET_LDFLAGS:=-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib
ifneq ($(CONFIG_EXTERNAL_TOOLCHAIN),)
LIBGCC_S_PATH=$(realpath $(wildcard $(call qstrip,$(CONFIG_LIBGCC_ROOT_DIR))/$(call qstrip,$(CONFIG_LIBGCC_FILE_SPEC))))
LIBGCC_S=$(if $(LIBGCC_S_PATH),-L$(dir $(LIBGCC_S_PATH)) -lgcc_s)
LIBGCC_A=$(realpath $(lastword $(wildcard $(dir $(LIBGCC_S_PATH))/gcc/*/*/libgcc.a)))
else
LIBGCC_A=$(lastword $(wildcard $(TOOLCHAIN_DIR)/lib/gcc/*/*/libgcc.a))
LIBGCC_S=$(if $(wildcard $(TOOLCHAIN_DIR)/lib/libgcc_s.so),-L$(TOOLCHAIN_DIR)/lib -lgcc_s,$(LIBGCC_A))
endif
LIBRPC=-lrpc
LIBRPC_DEPENDS=+librpc

ifeq ($(CONFIG_ARCH_64BIT),y)
  LIB_SUFFIX:=64
endif

ifndef DUMP
  ifeq ($(CONFIG_EXTERNAL_TOOLCHAIN),)
    -include $(TOOLCHAIN_DIR)/info.mk
    export GCC_HONOUR_COPTS:=0
    TARGET_CROSS:=$(if $(TARGET_CROSS),$(TARGET_CROSS),$(OPTIMIZE_FOR_CPU)-openwrt-linux$(if $(TARGET_SUFFIX),-$(TARGET_SUFFIX))-)
    TARGET_CFLAGS+= -fhonour-copts $(if $(CONFIG_GCC_VERSION_4_4)$(CONFIG_GCC_VERSION_4_5),,-Wno-error=unused-but-set-variable)
    TARGET_CPPFLAGS+= -I$(TOOLCHAIN_DIR)/usr/include -I$(TOOLCHAIN_DIR)/include
    TARGET_LDFLAGS+= -L$(TOOLCHAIN_DIR)/usr/lib -L$(TOOLCHAIN_DIR)/lib
    TARGET_PATH:=$(TOOLCHAIN_DIR)/bin:$(TARGET_PATH)
  else
    ifeq ($(CONFIG_NATIVE_TOOLCHAIN),)
      TARGET_CROSS:=$(call qstrip,$(CONFIG_TOOLCHAIN_PREFIX))
      TOOLCHAIN_ROOT_DIR:=$(call qstrip,$(CONFIG_TOOLCHAIN_ROOT))
      TOOLCHAIN_BIN_DIRS:=$(patsubst ./%,$(TOOLCHAIN_ROOT_DIR)/%,$(call qstrip,$(CONFIG_TOOLCHAIN_BIN_PATH)))
      TOOLCHAIN_INC_DIRS:=$(patsubst ./%,$(TOOLCHAIN_ROOT_DIR)/%,$(call qstrip,$(CONFIG_TOOLCHAIN_INC_PATH)))
      TOOLCHAIN_LIB_DIRS:=$(patsubst ./%,$(TOOLCHAIN_ROOT_DIR)/%,$(call qstrip,$(CONFIG_TOOLCHAIN_LIB_PATH)))
      ifneq ($(TOOLCHAIN_BIN_DIRS),)
        TARGET_PATH:=$(subst $(space),:,$(TOOLCHAIN_BIN_DIRS)):$(TARGET_PATH)
      endif
      ifneq ($(TOOLCHAIN_INC_DIRS),)
        TARGET_CPPFLAGS+= $(patsubst %,-I%,$(TOOLCHAIN_INC_DIRS))
      endif
      ifneq ($(TOOLCHAIN_LIB_DIRS),)
        TARGET_LDFLAGS+= $(patsubst %,-L%,$(TOOLCHAIN_LIB_DIRS))
      endif
      TARGET_PATH:=$(TOOLCHAIN_DIR)/bin:$(TARGET_PATH)
    endif
  endif
endif
TARGET_PATH_PKG:=$(STAGING_DIR)/host/bin:$(TARGET_PATH)

ifeq ($(CONFIG_SOFT_FLOAT),y)
  SOFT_FLOAT_CONFIG_OPTION:=--with-float=soft
  ifeq ($(CONFIG_arm),y)
    TARGET_CFLAGS+= -mfloat-abi=soft
  else
    TARGET_CFLAGS+= -msoft-float
  endif
else
  SOFT_FLOAT_CONFIG_OPTION:=
  ifeq ($(CONFIG_arm),y)
    TARGET_CFLAGS+= -mfloat-abi=hard
  endif
endif

export PATH:=$(TARGET_PATH)
export STAGING_DIR
export SH_FUNC:=. $(INCLUDE_DIR)/shell.sh;

PKG_CONFIG:=$(STAGING_DIR_HOST)/bin/pkg-config

export PKG_CONFIG

HOSTCC:=gcc
HOSTCXX:=g++
HOST_CPPFLAGS:=-I$(STAGING_DIR_HOST)/include
HOST_CFLAGS:=-O2 $(HOST_CPPFLAGS)
HOST_LDFLAGS:=-L$(STAGING_DIR_HOST)/lib

TARGET_CC:=$(TARGET_CROSS)gcc
TARGET_AR:=$(TARGET_CROSS)ar
TARGET_RANLIB:=$(TARGET_CROSS)ranlib
TARGET_CXX:=$(TARGET_CROSS)g++
KPATCH:=$(SCRIPT_DIR)/patch-kernel.sh
SED:=$(STAGING_DIR_HOST)/bin/sed -i -e
CP:=cp -fpR
LN:=ln -sf
XARGS:=xargs -r

INSTALL_BIN:=install -m0755
INSTALL_DIR:=install -d -m0755
INSTALL_DATA:=install -m0644
INSTALL_CONF:=install -m0600

TARGET_CC_NOCACHE:=$(TARGET_CC)
TARGET_CXX_NOCACHE:=$(TARGET_CXX)
HOSTCC_NOCACHE:=$(HOSTCC)
HOSTCXX_NOCACHE:=$(HOSTCXX)
export TARGET_CC_NOCACHE
export TARGET_CXX_NOCACHE
export HOSTCC_NOCACHE

ifneq ($(CONFIG_CCACHE),)
  TARGET_CC:= ccache_cc
  TARGET_CXX:= ccache_cxx
  HOSTCC:= ccache $(HOSTCC)
  HOSTCXX:= ccache $(HOSTCXX)
endif

TARGET_CONFIGURE_OPTS = \
  AR=$(TARGET_CROSS)ar \
  AS="$(TARGET_CC) -c $(TARGET_ASFLAGS)" \
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
    STRIP:=$(TARGET_CROSS)strip $(call qstrip,$(CONFIG_STRIP_ARGS))
  else
    ifneq ($(CONFIG_USE_SSTRIP),)
      STRIP:=$(STAGING_DIR_HOST)/bin/sstrip
    endif
  endif
  RSTRIP:= \
    export CROSS="$(TARGET_CROSS)" \
		$(if $(CONFIG_KERNEL_KALLSYMS),NO_RENAME=1) \
		$(if $(CONFIG_KERNEL_PROFILING),KEEP_SYMBOLS=1); \
    NM="$(TARGET_CROSS)nm" \
    STRIP="$(STRIP)" \
    STRIP_KMOD="$(SCRIPT_DIR)/strip-kmod.sh" \
    $(SCRIPT_DIR)/rstrip.sh
endif

ifeq ($(CONFIG_ENABLE_LOCALE),true)
  DISABLE_NLS:=--enable-nls
else
  DISABLE_NLS:=--disable-nls
endif

ifeq ($(CONFIG_IPV6),y)
  DISABLE_IPV6:=
else
  DISABLE_IPV6:=--disable-ipv6
endif

ifeq ($(CONFIG_BUILD_LOG),y)
  BUILD_LOG:=1
endif

define shvar
V_$(subst .,_,$(subst -,_,$(subst /,_,$(1))))
endef

define shexport
$(call shvar,$(1))=$$(call $(1))
export $(call shvar,$(1))
endef

define include_mk
$(eval -include $(if $(DUMP),,$(STAGING_DIR)/mk/$(strip $(1))))
endef

# Execute commands under flock
# $(1) => The shell expression.
# $(2) => The lock name. If not given, the global lock will be used.
define locked
	SHELL= \
	$(STAGING_DIR_HOST)/bin/flock \
		$(TMP_DIR)/.$(if $(2),$(strip $(2)),global).flock \
		-c '$(subst ','\'',$(1))'
endef

# Recursively copy paths into another directory, purge dangling
# symlinks before.
# $(1) => File glob expression
# $(2) => Destination directory
define file_copy
	for src_dir in $(sort $(foreach d,$(wildcard $(1)),$(dir $(d)))); do \
		( cd $$src_dir; find -type f -or -type d ) | \
			( cd $(2); while :; do \
				read FILE; \
				[ -z "$$FILE" ] && break; \
				[ -L "$$FILE" ] || continue; \
				echo "Removing symlink $(2)/$$FILE"; \
				rm -f "$$FILE"; \
			done; ); \
	done; \
	$(CP) $(1) $(2)
endef

# file extension
ext=$(word $(words $(subst ., ,$(1))),$(subst ., ,$(1)))

all:
FORCE: ;
.PHONY: FORCE

val.%:
	@$(if $(filter undefined,$(origin $*)),\
		echo "$* undefined" >&2, \
		echo '$(subst ','"'"',$($*))' \
	)

var.%:
	@$(if $(filter undefined,$(origin $*)),\
		echo "$* undefined" >&2, \
		echo "$*='"'$(subst ','"'\"'\"'"',$($*))'"'" \
	)

endif #__rules_inc

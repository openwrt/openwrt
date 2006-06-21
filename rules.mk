ifeq ($(DUMP),)
include $(TOPDIR)/.config
endif

SHELL=/bin/bash
export SHELL

ifdef V
  ifeq ("$(origin V)", "command line")
    KBUILD_VERBOSE = $(V)
  endif
endif
ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE = 0
endif

ifneq ($(KBUILD_VERBOSE),0)
  quiet =
  Q =
else
  quiet=quiet_
  Q = @
endif


CP=cp -fpR
BUILD_MAKEFLAGS= V=$(V) $(EXTRA_MAKEFLAGS)
MAKEFLAGS=$(BUILD_MAKEFLAGS)
# Strip off the annoying quoting
ARCH:=$(strip $(subst ",, $(CONFIG_ARCH)))
WGET:=$(strip $(subst ",, $(CONFIG_WGET)))
TARGET_OPTIMIZATION:=$(strip $(subst ",, $(CONFIG_TARGET_OPTIMIZATION)))
#"))"))"))"))")) # for vim's broken syntax highlighting :)

ifeq ($(CONFIG_TAR_VERBOSITY),y)
TAR_OPTIONS=-xvf
else
TAR_OPTIONS=-xf
endif

ifneq ($(CONFIG_LARGEFILE),y)
DISABLE_LARGEFILE= --disable-largefile
endif
TARGET_CFLAGS:=$(TARGET_OPTIMIZATION) $(TARGET_DEBUGGING)

OPTIMIZE_FOR_CPU=$(ARCH)
HOSTCC:=gcc
BASE_DIR:=$(TOPDIR)
INCLUDE_DIR:=$(TOPDIR)/include
DL_DIR:=$(BASE_DIR)/dl
BUILD_DIR:=$(BASE_DIR)/build_$(ARCH)
STAGING_DIR:=$(BASE_DIR)/staging_dir_$(ARCH)
SCRIPT_DIR:=$(BASE_DIR)/scripts
BIN_DIR:=$(BASE_DIR)/bin
STAMP_DIR:=$(BUILD_DIR)/stamp
PACKAGE_DIR:=$(BIN_DIR)/packages
STAMP_DIR:=$(BUILD_DIR)/stamp
TARGET_DIR:=$(BUILD_DIR)/root
TOOL_BUILD_DIR=$(BASE_DIR)/toolchain_build_$(ARCH)
TARGET_PATH=$(STAGING_DIR)/usr/bin:$(STAGING_DIR)/bin:/bin:/sbin:/usr/bin:/usr/sbin
IMAGE:=$(BUILD_DIR)/root_fs_$(ARCH)
REAL_GNU_TARGET_NAME=$(OPTIMIZE_FOR_CPU)-linux-uclibc
GNU_TARGET_NAME=$(OPTIMIZE_FOR_CPU)-linux
KERNEL_CROSS:=$(STAGING_DIR)/bin/$(OPTIMIZE_FOR_CPU)-linux-uclibc-
TARGET_CROSS:=$(STAGING_DIR)/bin/$(OPTIMIZE_FOR_CPU)-linux-uclibc-
TARGET_CC:=$(TARGET_CROSS)gcc
STRIP:=$(STAGING_DIR)/bin/sstrip
PATCH=$(SCRIPT_DIR)/patch-kernel.sh
SED:=$(STAGING_DIR)/bin/sed -i -e
LINUX_DIR:=$(BUILD_DIR)/linux
LINUX_HEADERS_DIR:=$(TOOL_BUILD_DIR)/linux


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
		RANLIB=$(TARGET_CROSS)ranlib \
		STRIP=$(TARGET_CROSS)strip

ifeq ($(ENABLE_LOCALE),true)
DISABLE_NLS:=
else
DISABLE_NLS:=--disable-nls
endif

ifeq ($(CONFIG_ENABLE_MULTILIB),y)
MULTILIB:=--enable-multilib
endif

# invoke ipkg-build with some default options
IPKG_BUILD := PATH="$(TARGET_PATH)" ipkg-build -c -o root -g root
# where to build (and put) .ipk packages
IPKG_TARGET_DIR := $(PACKAGE_DIR)
IPKG:=IPKG_TMP=$(BUILD_DIR)/tmp IPKG_INSTROOT=$(TARGET_DIR) IPKG_CONF_DIR=$(STAGING_DIR)/etc IPKG_OFFLINE_ROOT=$(BUILD_DIR)/root $(SCRIPT_DIR)/ipkg -force-defaults -force-depends
IPKG_STATE_DIR := $(TARGET_DIR)/usr/lib/ipkg

RSTRIP:=STRIP="$(STRIP)" STRIP_KMOD="$(TARGET_CROSS)strip --strip-unneeded --remove-section=.comment" $(SCRIPT_DIR)/rstrip.sh

all:
.PHONY: FORCE
FORCE:

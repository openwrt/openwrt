include $(TOPDIR)/.config
SHELL=/bin/bash
export SHELL

ifeq ($(V),)
V=5
endif


ifneq ($(V),0)
TRACE:=echo "---> "
START_TRACE:=echo -n "---> "
END_TRACE:=echo
else
START_TRACE:=:
END_TRACE:=:
TRACE:=:
endif

ifeq (${shell [ "$(V)" -ge 5 ] && echo 1},)
CMD_TRACE:=:
PKG_TRACE:=:
else
CMD_TRACE:=echo -n
PKG_TRACE:=echo "------> "
endif

ifeq (${shell [ "$(V)" -ge 10 ] && echo 1},)
EXTRA_MAKEFLAGS:=-s
MAKE_TRACE:=2>&1 >&/dev/null || { echo "Build failed. Please re-run make with V=99 to see what's going on"; /bin/false; }
else
MAKE_TRACE:=
EXTRA_MAKEFLAGS:=
TRACE:=:
PKG_TRACE:=:
CMD_TRACE:=:
START_TRACE:=:
END_TRACE:=:
endif

CP=cp -fpR
MAKE1=make
MAKEFLAGS=-j$(BR2_JLEVEL) V=$(V) $(EXTRA_MAKEFLAGS)
# Strip off the annoying quoting
ARCH:=$(strip $(subst ",, $(BR2_ARCH)))
WGET:=$(strip $(subst ",, $(BR2_WGET)))
GCC_VERSION:=$(strip $(subst ",, $(BR2_GCC_VERSION)))
GCC_USE_SJLJ_EXCEPTIONS:=$(strip $(subst ",, $(BR2_GCC_USE_SJLJ_EXCEPTIONS)))
TARGET_OPTIMIZATION:=$(strip $(subst ",, $(BR2_TARGET_OPTIMIZATION)))
#"))"))"))"))")) # for vim's broken syntax highlighting :)


ifeq ($(BR2_SOFT_FLOAT),y)
# gcc 3.4.x soft float configuration is different than previous versions.
ifeq ($(findstring 3.4.,$(GCC_VERSION)),3.4.)
SOFT_FLOAT_CONFIG_OPTION:=--with-float=soft
else
SOFT_FLOAT_CONFIG_OPTION:=--without-float
endif
TARGET_SOFT_FLOAT:=-msoft-float
ARCH_FPU_SUFFIX:=_nofpu
else
SOFT_FLOAT_CONFIG_OPTION:=
TARGET_SOFT_FLOAT:=
ARCH_FPU_SUFFIX:=
endif


ifeq ($(BR2_TAR_VERBOSITY),y)
TAR_OPTIONS=-xvf
else
TAR_OPTIONS=-xf
endif

ifneq ($(BR2_LARGEFILE),y)
DISABLE_LARGEFILE= --disable-largefile
endif
TARGET_CFLAGS:=$(TARGET_OPTIMIZATION) $(TARGET_DEBUGGING)

OPTIMIZE_FOR_CPU=$(ARCH)
HOSTCC:=gcc
BASE_DIR:=$(TOPDIR)
DL_DIR:=$(BASE_DIR)/dl
BUILD_DIR:=$(BASE_DIR)/build_$(ARCH)$(ARCH_FPU_SUFFIX)
STAGING_DIR:=$(BASE_DIR)/staging_dir_$(ARCH)$(ARCH_FPU_SUFFIX)
SCRIPT_DIR:=$(BASE_DIR)/scripts
BIN_DIR:=$(BASE_DIR)/bin
STAMP_DIR:=$(BUILD_DIR)/stamp
PACKAGE_DIR:=$(BIN_DIR)/packages
STAMP_DIR:=$(BUILD_DIR)/stamp
TARGET_DIR:=$(BUILD_DIR)/root
TOOL_BUILD_DIR=$(BASE_DIR)/toolchain_build_$(ARCH)$(ARCH_FPU_SUFFIX)
TARGET_PATH=$(STAGING_DIR)/usr/bin:$(STAGING_DIR)/bin:/bin:/sbin:/usr/bin:/usr/sbin
IMAGE:=$(BUILD_DIR)/root_fs_$(ARCH)$(ARCH_FPU_SUFFIX)
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
		RANLIB=$(TARGET_CROSS)ranlib

ifeq ($(ENABLE_LOCALE),true)
DISABLE_NLS:=
else
DISABLE_NLS:=--disable-nls
endif

ifeq ($(BR2_ENABLE_MULTILIB),y)
MULTILIB:=--enable-multilib
endif

# invoke ipkg-build with some default options
IPKG_BUILD := PATH="$(TARGET_PATH)" ipkg-build -c -o root -g root
# where to build (and put) .ipk packages
IPKG_TARGET_DIR := $(PACKAGE_DIR)
IPKG:=IPKG_TMP=$(BUILD_DIR)/tmp IPKG_INSTROOT=$(TARGET_DIR) IPKG_CONF_DIR=$(STAGING_DIR)/etc IPKG_OFFLINE_ROOT=$(BUILD_DIR)/root $(SCRIPT_DIR)/ipkg -force-defaults -force-depends
IPKG_STATE_DIR := $(TARGET_DIR)/usr/lib/ipkg

RSTRIP:=STRIP="$(STRIP)" $(SCRIPT_DIR)/rstrip.sh
RSTRIP_KMOD:=STRIP="$(TARGET_CROSS)strip --strip-unneeded --remove-section=.comment" $(SCRIPT_DIR)/rstrip.sh


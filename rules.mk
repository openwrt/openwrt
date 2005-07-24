include $(TOPDIR)/.config
MAKE1=make
MAKEFLAGS=-j$(BR2_JLEVEL)

# Strip off the annoying quoting
ARCH:=$(strip $(subst ",, $(BR2_ARCH)))
#"
WGET:=$(strip $(subst ",, $(BR2_WGET)))
#"

GCC_VERSION:=$(strip $(subst ",, $(BR2_GCC_VERSION)))
#"
GCC_USE_SJLJ_EXCEPTIONS:=$(strip $(subst ",, $(BR2_GCC_USE_SJLJ_EXCEPTIONS)))
#"
TARGET_OPTIMIZATION:=$(strip $(subst ",, $(BR2_TARGET_OPTIMIZATION)))
#""


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
SCRIPT_DIR:=$(BASE_DIR)/scripts
BIN_DIR:=$(BASE_DIR)/bin
STAMP_DIR:=$(BUILD_DIR)/stamp
PACKAGE_DIR:=$(BIN_DIR)/packages
STAMP_DIR:=$(BUILD_DIR)/stamp
TOOL_BUILD_DIR=$(BASE_DIR)/toolchain_build_$(ARCH)$(ARCH_FPU_SUFFIX)
# Strip off the annoying quoting
STAGING_DIR:=$(strip $(subst ",, $(BR2_STAGING_DIR)))
#"
TARGET_PATH=$(STAGING_DIR)/usr/bin:$(STAGING_DIR)/bin:/bin:/sbin:/usr/bin:/usr/sbin
IMAGE:=$(BUILD_DIR)/root_fs_$(ARCH)$(ARCH_FPU_SUFFIX)
REAL_GNU_TARGET_NAME=$(OPTIMIZE_FOR_CPU)-linux-uclibc
GNU_TARGET_NAME=$(OPTIMIZE_FOR_CPU)-linux
KERNEL_CROSS:=$(STAGING_DIR)/bin/$(OPTIMIZE_FOR_CPU)-linux-uclibc-
TARGET_CROSS:=$(STAGING_DIR)/bin/$(OPTIMIZE_FOR_CPU)-linux-uclibc-
TARGET_CC:=$(TARGET_CROSS)gcc
#STRIP:=$(TARGET_CROSS)strip --remove-section=.comment --remove-section=.note
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

RSTRIP:=STRIP="$(STRIP)" $(SCRIPT_DIR)/rstrip.sh
RSTRIP_KMOD:=STRIP="$(TARGET_CROSS)strip --strip-unneeded --remove-section=.comment" $(SCRIPT_DIR)/rstrip.sh


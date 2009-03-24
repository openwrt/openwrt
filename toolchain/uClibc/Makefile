#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/target.mk

PKG_NAME:=uClibc
PKG_VERSION:=$(call qstrip,$(CONFIG_UCLIBC_VERSION))
PKG_EXTRAVERSION:=$(call qstrip,$(CONFIG_UCLIBC_EXTRA_VERSION))
PKG_VERSION_SNAPSHOT:=$(call qstrip,$(CONFIG_UCLIBC_VERSION_SNAPSHOT))
ifeq ($(CONFIG_UCLIBC_VERSION_NPTL),y)
  PKG_SOURCE_URL:=http://www.uclibc.org/svn/branches/uClibc-nptl
  PKG_SOURCE_PROTO:=svn
  PKG_SOURCE_VERSION:=25804
  PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)+nptl
  PKG_SOURCE:=$(PKG_NAME)-nptl-r$(PKG_SOURCE_VERSION).tar.gz
  PKG_EXTRAVERSION:=
else
ifeq ($(PKG_VERSION_SNAPSHOT),y)
  PKG_SOURCE_URL:=http://www.uclibc.org/downloads/snapshots
  PKG_SOURCE:=uClibc-snapshot.tar.bz2
else
  PKG_SOURCE_URL:=http://www.uclibc.org/downloads
  PKG_SOURCE:=uClibc-$(PKG_VERSION)$(PKG_EXTRAVERSION).tar.bz2
endif
endif
PATCH_DIR:=$(firstword $(wildcard ./patches-$(PKG_VERSION)$(PKG_EXTRAVERSION) ./patches-$(PKG_VERSION)))
CONFIG_DIR:=$(firstword $(wildcard ./config-$(PKG_VERSION)$(PKG_EXTRAVERSION) ./config-$(PKG_VERSION)))
PATCH_DIR:=$(if $(CONFIG_UCLIBC_VERSION_NPTL),./patches-nptl,$(PATCH_DIR))
CONFIG_DIR:=$(if $(CONFIG_UCLIBC_VERSION_NPTL),./config-nptl,$(CONFIG_DIR))
PATCH_DIR:=$(if $(PKG_VERSION_SNAPSHOT),,$(PATCH_DIR))
CONFIG_DIR:=$(if $(PKG_VERSION_SNAPSHOT),./config-0.9.30,$(CONFIG_DIR))

PKG_MD5SUM_0.9.28.2 = 959f25286e317f0d9e2103445c5a14c2
PKG_MD5SUM_0.9.28.3 = 428405a36b4662980d9343b32089b5a6
PKG_MD5SUM_0.9.29 = 61dc55f43b17a38a074f347e74095b20
PKG_MD5SUM_0.9.30 = e5766e2566e0297adebebbcc0aba1f2d
PKG_MD5SUM=$(PKG_MD5SUM_$(PKG_VERSION)$(PKG_EXTRAVERSION))


ifeq ($(PKG_VERSION_SNAPSHOT),y)
  UCLIBC_SUBDIR:=uClibc-$(PKG_EXTRAVERSION)
else
  UCLIBC_SUBDIR:=uClibc-$(PKG_VERSION)$(PKG_EXTRAVERSION)$(if $(CONFIG_UCLIBC_VERSION_NPTL),+nptl)
endif
HOST_BUILD_DIR:=$(BUILD_DIR_TOOLCHAIN)/$(UCLIBC_SUBDIR)

include $(INCLUDE_DIR)/toolchain-build.mk

HOST_STAMP_BUILT:=$(TOOLCHAIN_DIR)/stamp/.uclibc_installed
HOST_STAMP_INSTALLED:=$(TOOLCHAIN_DIR)/stamp/.uclibc-utils_installed

UCLIBC_TARGET_ARCH:=$(shell echo $(ARCH) | sed -e s'/-.*//' \
		-e 's/i.86/i386/' \
		-e 's/sparc.*/sparc/' \
		-e 's/arm.*/arm/g' \
		-e 's/avr32.*/avr32/g' \
		-e 's/m68k.*/m68k/' \
		-e 's/ppc/powerpc/g' \
		-e 's/v850.*/v850/g' \
		-e 's/sh64/sh/' \
		-e 's/sh[234].*/sh/' \
		-e 's/mips.*/mips/' \
		-e 's/mipsel.*/mips/' \
		-e 's/cris.*/cris/' \
)


define Host/SetToolchainInfo
	$(SED) 's,^\(LIBC_TYPE\)=.*,\1=$(PKG_NAME),' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_URL\)=.*,\1=http://www.uclibc.org/,' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_VERSION\)=.*,\1=$(PKG_VERSION),' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,^\(LIBC_PATCHVER\)=.*,\1=$(PKG_EXTRAVERSION),' $(TOOLCHAIN_DIR)/info.mk
endef

define Host/Prepare/Snapshot
	$(PKG_UNPACK)
	rm -Rf $(HOST_BUILD_DIR)
	$(Host/Patch)
	$(if $(QUILT),touch $(HOST_BUILD_DIR)/.quilt_used)
endef

define Host/Prepare/V_0_9_28
	$(SED) 's,^KERNEL_SOURCE=.*,KERNEL_SOURCE=\"$(BUILD_DIR_TOOLCHAIN)/linux\",g' \
		$(HOST_BUILD_DIR)/.config
 ifeq ($(CONFIG_SOFT_FLOAT),y)
	$(SED) 's,.*HAS_FPU.*,HAS_FPU=n\nUCLIBC_HAS_FLOATS=y\nUCLIBC_HAS_SOFT_FLOAT=y,g' $(HOST_BUILD_DIR)/.config
 endif
 ifeq ($(CONFIG_C99_MATH),y)
	$(SED) 's,.*DO_C99_MATH.*,DO_C99_MATH=y,g' $(HOST_BUILD_DIR)/.config
 endif
endef

GEN_CONFIG=$(SCRIPT_DIR)/kconfig.pl -n $(if $(wildcard $(CONFIG_DIR)/common),'+' $(CONFIG_DIR)/common) ./$(CONFIG_DIR)/$(ARCH)$(if $(wildcard $(CONFIG_DIR)/$(ARCH).$(BOARD)),.$(BOARD))

define Host/Prepare
	$(call Host/SetToolchainInfo)
	@echo prepare uClibc version $(PKG_VERSION)$(PKG_EXTRAVERSION)  
 ifeq ($(PKG_VERSION_SNAPSHOT),y)
	$(call Host/Prepare/Snapshot)
 else
	$(call Host/Prepare/Default)
 endif
	ln -snf $(UCLIBC_SUBDIR) $(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
	$(GEN_CONFIG) > $(HOST_BUILD_DIR)/.config
 ifeq ($(PKG_VERSION),0.9.28)
	$(call Host/Prepare/V_0_9_28)
 endif
	$(SED) 's,^KERNEL_HEADERS=.*,KERNEL_HEADERS=\"$(BUILD_DIR_TOOLCHAIN)/linux-dev/usr/include\",g' \
		-e 's,^.*UCLIBC_HAS_FPU.*,UCLIBC_HAS_FPU=$(if $(CONFIG_SOFT_FLOAT),n,y),g' \
		-e 's,^.*UCLIBC_HAS_SOFT_FLOAT.*,UCLIBC_HAS_SOFT_FLOAT=$(if $(CONFIG_SOFT_FLOAT),y,n),g' \
		-e 's,^.*UCLIBC_HAS_LFS.*,UCLIBC_HAS_LFS=$(if $(CONFIG_LARGEFILE),y,n),g' \
		-e 's,^.*CONFIG_ARM_EABI.*,CONFIG_ARM_EABI=$(if $(CONFIG_EABI_SUPPORT),y,n),g' \
		-e 's,^.*CONFIG_ARM_OABI.*,CONFIG_ARM_OABI=$(if $(CONFIG_EABI_SUPPORT),n,y),g' \
		-e 's,^.*UCLIBC_HAS_SHADOW.*,UCLIBC_HAS_SHADOW=$(if $(CONFIG_SHADOW_PASSWORDS),y,n),g' \
		$(HOST_BUILD_DIR)/.config
	$(MAKE) -C $(HOST_BUILD_DIR)/extra/config conf KBUILD_HAVE_NLS= HOSTCFLAGS="-DKBUILD_NO_NLS"
	PATH=$(TARGET_PATH) $(MAKE) -C $(HOST_BUILD_DIR) \
		PREFIX="$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/" \
		DEVEL_PREFIX=/usr/ \
		RUNTIME_PREFIX="$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/" \
		HOSTCC="$(HOSTCC)" \
		CC="$(TARGET_CC)" \
		CPU_CFLAGS="$(TARGET_CFLAGS)" \
		pregen \
		$(if $(CONFIG_UCLIBC_VERSION_0_9_28),install_dev,install_headers)
endef

define Host/Configure
endef

UCLIBC_MAKE := PATH=$(TARGET_PATH) $(MAKE) -C $(HOST_BUILD_DIR) \
	$(TARGET_CONFIGURE_OPTS) \
	DEVEL_PREFIX=/usr/ \
	RUNTIME_PREFIX=/ \
	HOSTCC="$(HOSTCC)" \
	CPU_CFLAGS="$(TARGET_CFLAGS)" \
	DOSTRIP=""

define Host/Compile
	$(SED) 's,^CROSS=.*,CROSS=$(TARGET_CROSS),g' $(HOST_BUILD_DIR)/Rules.mak
	$(UCLIBC_MAKE) PREFIX= all
	$(UCLIBC_MAKE) PREFIX="$(TOOLCHAIN_DIR)/" install_runtime install_dev
	ln -sf ../../lib/libc.so.0 $(TOOLCHAIN_DIR)/usr/lib/libc.so
#	( cd $(TOOLCHAIN_DIR) ; \
#		for d in lib usr/lib ; do \
#		  for f in libc.so libpthread.so libgcc_s.so ; do \
#		    if [ -f $$$$d/$$$$f -a ! -L $$$$d/$$$$f ] ; then \
#		      $(SED) 's,/usr/lib/,,g;s,/lib/,,g' $$$$d/$$$$f ; \
#		    fi \
#		  done \
#		done \
#	)
endef

define Host/Install
	$(UCLIBC_MAKE) PREFIX= utils
	$(INSTALL_DIR) $(TOOLCHAIN_DIR)/usr/bin
	$(INSTALL_BIN) \
		$(HOST_BUILD_DIR)/utils/ldd \
		$(TOOLCHAIN_DIR)/usr/bin/
	$(INSTALL_DIR) $(TOOLCHAIN_DIR)/sbin
	$(INSTALL_BIN) \
		$(HOST_BUILD_DIR)/utils/ldconfig \
		$(TOOLCHAIN_DIR)/sbin/
endef

define Host/Clean
	rm -rf \
		$(HOST_BUILD_DIR) \
		$(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME) \
		$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev
endef

$(eval $(call HostBuild))

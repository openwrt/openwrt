# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/target.mk

PKG_NAME:=uClibc
PKG_VERSION:=0.9.29
PKG_EXTRAVERSION:=

PKG_SOURCE:=uClibc-$(PKG_VERSION)$(PKG_EXTRAVERSION).tar.bz2
PKG_SOURCE_URL:=http://www.uclibc.org/downloads
PKG_MD5SUM:=61dc55f43b17a38a074f347e74095b20
PKG_CAT:=bzcat

STAGING_DIR_HOST:=$(TOOLCHAIN_DIR)
BUILD_DIR_HOST:=$(BUILD_DIR_TOOLCHAIN)
PKG_BUILD_DIR:=$(BUILD_DIR_HOST)/uClibc-$(PKG_VERSION)$(PKG_EXTRAVERSION)

include $(INCLUDE_DIR)/host-build.mk

STAMP_BUILT:=$(TOOLCHAIN_DIR)/stamp/.uclibc_installed
STAMP_INSTALLED:=$(TOOLCHAIN_DIR)/stamp/.uclibc-utils_installed

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

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) config/$(ARCH)$(if $(wildcard config/$(ARCH).$(BOARD)),$(BOARD)) $(PKG_BUILD_DIR)/.config
	$(SED) 's,^KERNEL_HEADERS=.*,KERNEL_HEADERS=\"$(LINUX_HEADERS_DIR)/include\",g' \
		-e 's,.*UCLIBC_HAS_FPU.*,UCLIBC_HAS_FPU=$(if $(CONFIG_SOFT_FLOAT),n\nUCLIBC_HAS_FLOATS=y\nUCLIBC_HAS_SOFT_FLOAT=y,y),g' \
		-e 's,^.*UCLIBC_HAS_LFS.*,UCLIBC_HAS_LFS=$(if $(CONFIG_LARGEFILE),y,n),g' \
		$(PKG_BUILD_DIR)/.config
	mkdir -p $(BUILD_DIR_HOST)/uClibc_dev/usr/include
	mkdir -p $(BUILD_DIR_HOST)/uClibc_dev/usr/lib
	mkdir -p $(BUILD_DIR_HOST)/uClibc_dev/lib
#	if [ ! -f $(BUILD_DIR_HOST)/uClibc_dev/usr/include/linux/version.h ] ; then \
#		cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux/include/* $(BUILD_DIR_HOST)/uClibc_dev/usr/include/; \
#		cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux/include/* $(TOOLCHAIN_DIR)/include/ ; \
#		cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux/include/* $(PKG_BUILD_DIR)/include/ ; \
#	fi;
	if [ ! -f $(BUILD_DIR_HOST)/uClibc_dev/usr/include/linux/version.h ] ; then \
		cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux/include/asm $(BUILD_DIR_HOST)/uClibc_dev/usr/include/; \
		cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux/include/asm-$(UCLIBC_TARGET_ARCH) $(BUILD_DIR_HOST)/uClibc_dev/usr/include/; \
		cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux/include/asm-generic $(BUILD_DIR_HOST)/uClibc_dev/usr/include/; \
		cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux/include/linux $(BUILD_DIR_HOST)/uClibc_dev/usr/include/; \
		cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux/include/asm $(TOOLCHAIN_DIR)/include/ ; \
		cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux/include/asm-$(UCLIBC_TARGET_ARCH) $(TOOLCHAIN_DIR)/include/ ; \
		cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux/include/asm-generic $(TOOLCHAIN_DIR)/include/ ; \
		cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux/include/linux $(TOOLCHAIN_DIR)/include/ ; \
		cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux/include/asm $(PKG_BUILD_DIR)/include/ ; \
		cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux/include/asm-$(UCLIBC_TARGET_ARCH) $(PKG_BUILD_DIR)/include/ ; \
		cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux/include/asm-generic $(PKG_BUILD_DIR)/include/ ; \
		cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux/include/linux $(PKG_BUILD_DIR)/include/ ; \
	fi;
	PATH=$(TARGET_PATH) $(MAKE) -C $(PKG_BUILD_DIR) \
		PREFIX=$(BUILD_DIR_HOST)/uClibc_dev/ \
		DEVEL_PREFIX=/usr/ \
		RUNTIME_PREFIX=$(BUILD_DIR_HOST)/uClibc_dev/ \
		HOSTCC="$(HOSTCC)" \
		CPU_CFLAGS="$(TARGET_CFLAGS)" \
		pregen install_dev;
endef

define Build/Configure
endef

UCLIBC_MAKE := PATH=$(TARGET_PATH) $(MAKE) -C $(PKG_BUILD_DIR) \
	$(TARGET_CONFIGURE_OPTS) \
	DEVEL_PREFIX=/ \
	RUNTIME_PREFIX=/ \
	HOSTCC="$(HOSTCC)" \
	CPU_CFLAGS="$(TARGET_CFLAGS)"

define Build/Compile
	$(SED) 's,^CROSS=.*,CROSS=$(TARGET_CROSS),g' $(PKG_BUILD_DIR)/Rules.mak
	$(UCLIBC_MAKE) PREFIX= all 
	$(UCLIBC_MAKE) PREFIX=$(STAGING_DIR_HOST)/ install_runtime install_dev
	$(SED) 's,UCLIBC_VERSION=.*,UCLIBC_VERSION=$(PKG_VERSION),' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,UCLIBC_PATCHVER=.*,UCLIBC_PATCHVER=$(PKG_EXTRAVERSION),' $(TOOLCHAIN_DIR)/info.mk
	rm -rf $(TOOLCHAIN_DIR)/lib/libc.so
	ln -s $(TOOLCHAIN_DIR)/lib/libc.so.0 $(TOOLCHAIN_DIR)/lib/libc.so
endef

define Build/Install
	$(UCLIBC_MAKE) PREFIX= utils
	$(INSTALL_DIR) $(TOOLCHAIN_DIR)/target-utils
	$(INSTALL_BIN) \
		$(PKG_BUILD_DIR)/utils/ldd \
		$(PKG_BUILD_DIR)/utils/ldconfig \
		$(TOOLCHAIN_DIR)/target-utils/
endef

define Build/Clean
	rm -rf $(PKG_BUILD_DIR) $(BUILD_DIR_HOST)/uClibc_dev
endef

$(eval $(call HostBuild))

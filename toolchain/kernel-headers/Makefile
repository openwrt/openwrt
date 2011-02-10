# 
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

KERNEL_BUILD_DIR := $(BUILD_DIR_TOOLCHAIN)
BUILD_DIR := $(KERNEL_BUILD_DIR)

override QUILT:=
override HOST_QUILT:=

include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=linux
PKG_VERSION:=$(LINUX_VERSION)
PKG_SOURCE:=$(LINUX_SOURCE)
PKG_SOURCE_URL:=$(LINUX_SITE)
HOST_BUILD_DIR:=$(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)
PKG_MD5SUM:=$(LINUX_KERNEL_MD5SUM)
LINUX_DIR := $(HOST_BUILD_DIR)
FILES_DIR := 
PATCH_DIR := ./patches$(if $(wildcard ./patches-$(KERNEL_PATCHVER)),-$(KERNEL_PATCHVER))

include $(INCLUDE_DIR)/toolchain-build.mk
include $(INCLUDE_DIR)/kernel-defaults.mk

ifeq ($(strip $(BOARD)),uml)
  LINUX_KARCH:=$(subst x86_64,x86,$(subst i386,x86,$(ARCH)))
endif

LINUX_HAS_HEADERS_INSTALL:=y

KMAKE := $(MAKE) -C $(HOST_BUILD_DIR) \
	ARCH=$(LINUX_KARCH) \
	CC="$(KERNEL_CC)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	CROSS_COMPILE=$(TARGET_CROSS) \
	KBUILD_HAVE_NLS=no \
	CONFIG_SHELL=$(BASH)

define Host/Prepare/all
	mkdir -p $(BUILD_DIR_TOOLCHAIN)/linux-dev
	$(KMAKE) \
		INSTALL_HDR_PATH="$(BUILD_DIR_TOOLCHAIN)/linux-dev/" \
		headers_install
endef

# XXX: the following is needed to build lzma-loader
ifneq ($(CONFIG_mips)$(CONFIG_mipsel),)
  define Host/Prepare/lzma
	$(CP) \
		$(HOST_BUILD_DIR)/arch/mips/include/asm/asm.h \
		$(HOST_BUILD_DIR)/arch/mips/include/asm/regdef.h \
		$(BUILD_DIR_TOOLCHAIN)/linux-dev/include/asm/
  endef
endif

define Host/Prepare/post/cris
	$(CP) \
		$(HOST_BUILD_DIR)/include/linux/user.h \
		$(BUILD_DIR_TOOLCHAIN)/linux-dev/include/linux/
	ln -snf $(BUILD_DIR_TOOLCHAIN)/linux-dev/include/arch-v10/arch \
		$(BUILD_DIR_TOOLCHAIN)/linux-dev/include/arch
	$(SED) '/#include <asm\/page\.h>/d' $(BUILD_DIR_TOOLCHAIN)/linux-dev/include/asm/user.h
endef

define Host/Prepare/post/ubicom32
	$(CP) \
		$(HOST_BUILD_DIR)/arch/ubicom32/include/asm/elf.h \
		$(HOST_BUILD_DIR)/arch/ubicom32/include/asm/user.h \
		$(HOST_BUILD_DIR)/arch/ubicom32/include/asm/page.h \
		$(HOST_BUILD_DIR)/arch/ubicom32/include/asm/page_offset.h \
		$(BUILD_DIR_TOOLCHAIN)/linux-dev/include/asm/
endef

define Host/Prepare/post/mips
	$(call Host/Prepare/lzma)
endef

define Host/Prepare/post/mipsel
	$(call Host/Prepare/lzma)
endef

define Host/Prepare
	$(call Kernel/Prepare/Default)
	ln -sf linux-$(LINUX_VERSION) $(BUILD_DIR_TOOLCHAIN)/linux
	$(SED) 's/@expr length/@-expr length/' $(HOST_BUILD_DIR)/Makefile
	yes '' | $(KMAKE) oldconfig
	$(call Host/Prepare/all)
	$(call Host/Prepare/post/$(ARCH))
endef

define Host/Configure
endef

define Host/Compile
endef

define Host/Install
	$(CP) $(BUILD_DIR_TOOLCHAIN)/linux-dev/* $(TOOLCHAIN_DIR)/
endef

define Host/Clean
	rm -rf \
		$(HOST_BUILD_DIR) \
		$(BUILD_DIR_TOOLCHAIN)/linux \
		$(BUILD_DIR_TOOLCHAIN)/linux-dev
endef

$(eval $(call HostBuild))

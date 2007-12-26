# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

KERNEL_BUILD_DIR := $(BUILD_DIR_TOOLCHAIN)
BUILD_DIR := $(KERNEL_BUILD_DIR)

STAGING_DIR_HOST:=$(TOOLCHAIN_DIR)
BUILD_DIR_HOST:=$(BUILD_DIR_TOOLCHAIN)
override QUILT:=

include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=linux
PKG_VERSION:=$(LINUX_VERSION)
PKG_SOURCE:=$(LINUX_SOURCE)
PKG_SOURCE_URL:=$(LINUX_SITE)
PKG_BUILD_DIR := $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)
PKG_MD5SUM:=$(LINUX_KERNEL_MD5SUM)
LINUX_DIR := $(PKG_BUILD_DIR)

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/kernel-defaults.mk

define Build/Prepare/cris
	mkdir -p $(BUILD_DIR_TOOLCHAIN)
	bzcat $(DL_DIR)/$(PKG_SOURCE) | $(HOST_TAR) -C $(BUILD_DIR_TOOLCHAIN) $(TAR_OPTIONS)
	$(PATCH) $(PKG_BUILD_DIR) $(TOPDIR)/target/linux/etrax-2.6/patches/generic_2.6
	$(PATCH) $(PKG_BUILD_DIR) $(TOPDIR)/target/linux/etrax-2.6/patches/cris
	ln -sf arch-v10 $(PKG_BUILD_DIR)/include/asm-cris/arch
	ln -sf arch-v10 $(PKG_BUILD_DIR)/arch/cris/arch
endef

define Build/Configure/powerpc
	$(CP) $(PKG_BUILD_DIR)/include/asm-ppc/* $(PKG_BUILD_DIR)/include/asm-powerpc/
	rm -rf $(PKG_BUILD_DIR)/include/asm-ppc
	mv $(PKG_BUILD_DIR)/include/asm-powerpc $(PKG_BUILD_DIR)/include/asm-ppc
endef

ifneq (,$(findstring uml,$(BOARD)))
  LINUX_KARCH:=$(ARCH)
endif

KMAKE := $(MAKE) -C $(PKG_BUILD_DIR) \
	ARCH=$(LINUX_KARCH) \
	KBUILD_HAVE_NLS=no \
	CONFIG_SHELL=$(BASH)

define Build/Prepare
	$(if $(Build/Prepare/$(ARCH)),$(Build/Prepare/$(ARCH)),$(call Kernel/Prepare/Default))
	$(SED) 's/@expr length/@-expr length/' $(PKG_BUILD_DIR)/Makefile
	ln -sf linux-$(LINUX_VERSION) $(LINUX_HEADERS_DIR)
	yes '' | $(KMAKE) oldconfig
	$(KMAKE) include/linux/version.h include/asm
	$(call Build/Configure/$(ARCH))
endef

define Build/Configure
endef

define Build/Compile
endef

define Build/Install
endef

$(eval $(call HostBuild))

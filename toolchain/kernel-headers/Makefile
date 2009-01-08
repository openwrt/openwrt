# 
# Copyright (C) 2006-2008 OpenWrt.org
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
override CONFIG_AUTOREBUILD=

include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=linux
PKG_VERSION:=$(LINUX_VERSION)
PKG_SOURCE:=$(LINUX_SOURCE)
PKG_SOURCE_URL:=$(LINUX_SITE)
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)
PKG_MD5SUM:=$(LINUX_KERNEL_MD5SUM)
LINUX_DIR := $(PKG_BUILD_DIR)

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/kernel-defaults.mk

ifeq ($(strip $(BOARD)),uml)
  LINUX_KARCH:=$(ARCH)
endif

KMAKE := $(MAKE) -C $(PKG_BUILD_DIR) \
	ARCH=$(LINUX_KARCH) \
	KBUILD_HAVE_NLS=no \
	CONFIG_SHELL=$(BASH)

define Build/Prepare/pre/cris
	ln -sf $(PKG_BUILD_DIR)/include/asm-cris/arch-v10 $(PKG_BUILD_DIR)/include/asm-cris/arch
	ln -sf $(PKG_BUILD_DIR)/include/asm-cris/arch-v10 $(PKG_BUILD_DIR)/arch/cris/arch
endef

define Build/Prepare/pre/powerpc
	if [ -d $(PKG_BUILD_DIR)/include/asm-ppc ]; then \
		$(CP) $(PKG_BUILD_DIR)/include/asm-ppc/* $(PKG_BUILD_DIR)/include/asm-powerpc/; \
		rm -rf $(PKG_BUILD_DIR)/include/asm-ppc; \
		ln -s $(PKG_BUILD_DIR)/include/asm-powerpc $(PKG_BUILD_DIR)/include/asm-ppc; \
	fi
endef

ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.18)),1)
  define Build/Prepare/all
	mkdir -p $(BUILD_DIR_TOOLCHAIN)/linux-dev/usr
	$(KMAKE) \
		CROSS_COMPILE=$(TARGET_CROSS) \
		INSTALL_HDR_PATH="$(BUILD_DIR_TOOLCHAIN)/linux-dev/usr/" \
		headers_install
  endef
else
  define Build/Prepare/all
	mkdir -p $(BUILD_DIR_TOOLCHAIN)/linux-dev/usr/include
	cp -pLR \
		$(BUILD_DIR_TOOLCHAIN)/linux/include/asm \
		$(BUILD_DIR_TOOLCHAIN)/linux/include/asm-generic \
		$(BUILD_DIR_TOOLCHAIN)/linux/include/asm-$(LINUX_KARCH) \
		$(BUILD_DIR_TOOLCHAIN)/linux/include/linux \
		$(BUILD_DIR_TOOLCHAIN)/linux-dev/usr/include/
  endef
endif

define Build/Prepare
	$(call Kernel/Prepare/Default)
	$(SED) 's/@expr length/@-expr length/' $(PKG_BUILD_DIR)/Makefile
	ln -sf linux-$(LINUX_VERSION) $(LINUX_HEADERS_DIR)
	yes '' | $(KMAKE) oldconfig
	$(KMAKE) include/linux/version.h include/asm
	if [ -d $(PKG_BUILD_DIR)/arch/$(LINUX_KARCH)/include/asm ]; then \
		$(CP) \
			$(PKG_BUILD_DIR)/arch/$(LINUX_KARCH)/include/asm/. \
			$(PKG_BUILD_DIR)/include/asm-$(LINUX_KARCH)/; \
	fi
	$(call Build/Prepare/pre/$(ARCH))
	$(call Build/Prepare/all)
endef

define Build/Configure
endef

define Build/Compile
endef

define Build/Install/all
	mkdir -p $(TOOLCHAIN_DIR)/usr/include
	$(CP) \
		$(BUILD_DIR_TOOLCHAIN)/linux-dev/usr/include/* \
		$(TOOLCHAIN_DIR)/usr/include/
endef

# XXX: the following is needed to build lzma-loader
define Build/Install/post/lzma
	$(CP) \
		$(BUILD_DIR_TOOLCHAIN)/linux/include/asm-mips/asm.h \
		$(BUILD_DIR_TOOLCHAIN)/linux/include/asm-mips/regdef.h \
		$(TOOLCHAIN_DIR)/usr/include/asm/
endef

define Build/Install/post/mips
	$(call Build/Install/post/lzma)
endef

define Build/Install/post/mipsel
	$(call Build/Install/post/lzma)
endef

define Build/Install
	$(call Build/Install/all)
	$(call Build/Install/post/$(ARCH))
endef

define Build/Clean
	rm -rf \
		$(PKG_BUILD_DIR) \
		$(BUILD_DIR_TOOLCHAIN)/linux \
		$(BUILD_DIR_TOOLCHAIN)/linux-dev
endef

$(eval $(call HostBuild))

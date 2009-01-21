# 
# Copyright (C) 2006-2009 OpenWrt.org
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
		$(PKG_BUILD_DIR)/include/asm \
		$(PKG_BUILD_DIR)/include/asm-generic \
		$(PKG_BUILD_DIR)/include/asm-$(LINUX_KARCH) \
		$(PKG_BUILD_DIR)/include/linux \
		$(BUILD_DIR_TOOLCHAIN)/linux-dev/usr/include/
  endef
endif

# XXX: the following is needed to build lzma-loader
define Build/Prepare/lzma
	$(CP) \
		$(PKG_BUILD_DIR)/include/asm-mips/asm.h \
		$(PKG_BUILD_DIR)/include/asm-mips/regdef.h \
		$(BUILD_DIR_TOOLCHAIN)/linux-dev/usr/include/asm/
endef

# XXX: the following are needed to build a cris toolchain
define Build/Prepare/post/cris
	$(CP) \
		$(PKG_BUILD_DIR)/include/linux/user.h \
		$(BUILD_DIR_TOOLCHAIN)/linux-dev/usr/include/linux/
	$(CP) \
		$(PKG_BUILD_DIR)/include/asm-cris/elf.h \
		$(PKG_BUILD_DIR)/include/asm-cris/page.h \
		$(PKG_BUILD_DIR)/include/asm-cris/user.h \
		$(BUILD_DIR_TOOLCHAIN)/linux-dev/usr/include/asm/
	mkdir -p $(BUILD_DIR_TOOLCHAIN)/linux-dev/usr/include/asm/arch
	$(CP) \
		$(PKG_BUILD_DIR)/include/asm-cris/arch/elf.h \
		$(PKG_BUILD_DIR)/include/asm-cris/arch/page.h \
		$(PKG_BUILD_DIR)/include/asm-cris/arch/ptrace.h \
		$(PKG_BUILD_DIR)/include/asm-cris/arch/user.h \
		$(BUILD_DIR_TOOLCHAIN)/linux-dev/usr/include/asm/arch/
	$(CP) \
		$(PKG_BUILD_DIR)/include/asm-generic/memory_model.h \
		$(PKG_BUILD_DIR)/include/asm-generic/page.h \
		$(BUILD_DIR_TOOLCHAIN)/linux-dev/usr/include/asm-generic/
endef

define Build/Prepare/post/mips
	$(call Build/Prepare/lzma)
endef

define Build/Prepare/post/mipsel
	$(call Build/Prepare/lzma)
endef

define Build/Prepare
	$(call Kernel/Prepare/Default)
	ln -sf linux-$(LINUX_VERSION) $(BUILD_DIR_TOOLCHAIN)/linux
	$(SED) 's/@expr length/@-expr length/' $(PKG_BUILD_DIR)/Makefile
	yes '' | $(KMAKE) oldconfig
	$(KMAKE) include/linux/version.h include/asm
	if [ -d $(PKG_BUILD_DIR)/arch/$(LINUX_KARCH)/include/asm ]; then \
		$(CP) \
			$(PKG_BUILD_DIR)/arch/$(LINUX_KARCH)/include/asm/. \
			$(PKG_BUILD_DIR)/include/asm-$(LINUX_KARCH)/; \
	fi
	$(call Build/Prepare/pre/$(ARCH))
	$(call Build/Prepare/all)
	$(call Build/Prepare/post/$(ARCH))
endef

define Build/Configure
endef

define Build/Compile
endef

define Build/Install
	$(CP) $(BUILD_DIR_TOOLCHAIN)/linux-dev/* $(TOOLCHAIN_DIR)/
endef

define Build/Clean
	rm -rf \
		$(PKG_BUILD_DIR) \
		$(BUILD_DIR_TOOLCHAIN)/linux \
		$(BUILD_DIR_TOOLCHAIN)/linux-dev
endef

$(eval $(call HostBuild))

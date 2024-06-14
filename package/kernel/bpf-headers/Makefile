#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

override QUILT:=
override HOST_QUILT:=

include $(INCLUDE_DIR)/kernel.mk


PKG_NAME:=linux
PKG_PATCHVER:=5.15
# Manually include kernel version and hash from kernel details file
include $(INCLUDE_DIR)/kernel-$(PKG_PATCHVER)

PKG_VERSION:=$(PKG_PATCHVER)$(strip $(LINUX_VERSION-$(PKG_PATCHVER)))
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@KERNEL/linux/kernel/v$(word 1,$(subst ., ,$(PKG_PATCHVER))).x
PKG_HASH:=$(LINUX_KERNEL_HASH-$(strip $(PKG_VERSION)))
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/bpf-headers/$(PKG_NAME)-$(PKG_VERSION)

GENERIC_PLATFORM_DIR := $(CURDIR)/../../../target/linux/generic
GENERIC_BACKPORT_DIR := $(GENERIC_PLATFORM_DIR)/backport$(if $(wildcard $(GENERIC_PLATFORM_DIR)/backport-$(PKG_PATCHVER)),-$(PKG_PATCHVER))
GENERIC_PATCH_DIR := $(GENERIC_PLATFORM_DIR)/pending$(if $(wildcard $(GENERIC_PLATFORM_DIR)/pending-$(PKG_PATCHVER)),-$(PKG_PATCHVER))
GENERIC_HACK_DIR := $(GENERIC_PLATFORM_DIR)/hack$(if $(wildcard $(GENERIC_PLATFORM_DIR)/hack-$(PKG_PATCHVER)),-$(PKG_PATCHVER))
GENERIC_FILES_DIR := $(foreach dir,$(wildcard $(GENERIC_PLATFORM_DIR)/files $(GENERIC_PLATFORM_DIR)/files-$(PKG_PATCHVER)),"$(dir)")
PATCH_DIR := $(CURDIR)/patches
FILES_DIR :=

REAL_LINUX_DIR := $(LINUX_DIR)
LINUX_DIR := $(PKG_BUILD_DIR)

include $(INCLUDE_DIR)/bpf.mk
include $(INCLUDE_DIR)/package.mk

define Package/bpf-headers
  SECTION:=kernel
  CATEGORY:=Kernel modules
  TITLE:=eBPF kernel headers
  BUILDONLY:=1
  HIDDEN:=1
endef

PKG_CONFIG_PATH:=

export HOST_EXTRACFLAGS=-I$(STAGING_DIR_HOST)/include

KERNEL_MAKE := \
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH=$(BPF_KARCH) \
		CROSS_COMPILE=$(BPF_ARCH)-linux- \
		LLVM=1 CC="$(CLANG)" LD="$(TARGET_CROSS)ld" \
		HOSTCC="$(HOSTCC)" \
		HOSTCXX="$(HOSTCXX)" \
		KBUILD_HOSTLDLIBS="-L$(STAGING_DIR_HOST)/lib" \
		CONFIG_SHELL="$(BASH)" \
		INSTALL_HDR_PATH="$(PKG_BUILD_DIR)/user_headers"

define Build/Patch
	$(Kernel/Patch/Default)
endef

BPF_DOC = $(PKG_BUILD_DIR)/scripts/bpf_doc.py

define Build/Configure/64
	echo 'CONFIG_CPU_MIPS64_R2=y' >> $(PKG_BUILD_DIR)/.config
	echo 'CONFIG_64BIT=y' >> $(PKG_BUILD_DIR)/.config
endef

define Build/Configure
	grep -vE 'CONFIG_(CPU_.*ENDIAN|HZ)' $(PKG_BUILD_DIR)/arch/mips/configs/generic_defconfig > $(PKG_BUILD_DIR)/.config
	echo 'CONFIG_CPU_$(if $(CONFIG_BIG_ENDIAN),BIG,LITTLE)_ENDIAN=y' >> $(PKG_BUILD_DIR)/.config
	$(if $(CONFIG_ARCH_64BIT),$(Build/Configure/64))
	grep CONFIG_HZ $(REAL_LINUX_DIR)/.config >> $(PKG_BUILD_DIR)/.config
	yes '' | $(KERNEL_MAKE) oldconfig
	grep 'CONFIG_HZ=' $(REAL_LINUX_DIR)/.config | \
		cut -d= -f2 | \
		bc -q $(LINUX_DIR)/kernel/time/timeconst.bc \
		> $(LINUX_DIR)/include/generated/timeconst.h
	$(BPF_DOC) --header \
		--file $(LINUX_DIR)/tools/include/uapi/linux/bpf.h \
		> $(PKG_BUILD_DIR)/tools/lib/bpf/bpf_helper_defs.h
endef

SRC_BPFTOOL = $(REAL_LINUX_DIR)/tools/bpf/bpftool
DST_BPFTOOL = $(PKG_BUILD_DIR)/bpf_tool
MAKE_BPFTOOL = $(MAKE) -C $(SRC_BPFTOOL)
BPFTOOL = $(DST_BPFTOOL)/bpftool

VMLINUX = $(REAL_LINUX_DIR)/vmlinux
KERNEL_CONFIG = $(REAL_LINUX_DIR)/.config

VMLINUX_H_TEMP = $(PKG_BUILD_DIR)/vmlinux.h
VMLINUX_H_DEST = $(PKG_BUILD_DIR)/include/bpf/vmlinux.h

define Build/RemoveVmlinuxHeader
	rm -f $(VMLINUX_H_DEST)
endef
ifdef CONFIG_KERNEL_DEBUG_INFO_BTF
  define Build/GenerateVmlinuxHeader
	@[ -s '$(VMLINUX)' ] || { echo ; echo 'missing "vmlinux" in $(dir $(VMLINUX))' >&2 ; echo ; exit 1 ; }
	grep -Fxq CONFIG_DEBUG_INFO_BTF=y $(KERNEL_CONFIG) || { echo ; echo 'Kernel is built without CONFIG_DEBUG_INFO_BTF' >&2 ; echo ; exit 1 ; }
	@rm -rf $(DST_BPFTOOL) ; mkdir -p $(DST_BPFTOOL)
	+$(MAKE_BPFTOOL) O=$(DST_BPFTOOL) \
	  $(if $(findstring c,$(OPENWRT_VERBOSE)),V=1,V='') \
	  CC='$(HOSTCC)' LD='$(HOSTCC)' \
	  EXTRA_CFLAGS='$(HOST_CFLAGS) -I$(STAGING_DIR_HOSTPKG)/include -I$(STAGING_DIR)/host/include' \
	  LDFLAGS='$(HOST_LDFLAGS) -L$(STAGING_DIR_HOSTPKG)/lib -L$(STAGING_DIR)/host/lib' \
	  CROSS_COMPILE=
	$(BPFTOOL) btf dump file $(VMLINUX) format c > $(VMLINUX_H_TEMP)
  endef
else
  define Build/GenerateVmlinuxHeader
	echo '#error "Kernel is built without CONFIG_DEBUG_INFO_BTF, no type info available"' > $(VMLINUX_H_TEMP)
  endef
endif
define Build/InstallVmlinuxHeader
	@if [ -s '$(VMLINUX_H_DEST)' ] ; then echo ; echo '"include/bpf/vmlinux.h" already exists in $(PKG_BUILD_DIR)/' >&2 ; echo ; exit 1 ; fi
	mkdir -p $(dir $(VMLINUX_H_DEST))
	$(CP) $(VMLINUX_H_TEMP) $(dir $(VMLINUX_H_DEST))
endef

define Build/Compile
	$(Build/RemoveVmlinuxHeader)
	$(KERNEL_MAKE) archprepare headers_install
	$(Build/GenerateVmlinuxHeader)
	$(Build/InstallVmlinuxHeader)
endef

define Build/InstallDev
	mkdir -p $(1)/bpf-headers/arch $(1)/bpf-headers/tools
	$(CP) \
		$(PKG_BUILD_DIR)/arch/$(BPF_KARCH) \
		$(1)/bpf-headers/arch/
	$(CP) \
		$(PKG_BUILD_DIR)/tools/lib \
		$(PKG_BUILD_DIR)/tools/testing \
		$(1)/bpf-headers/tools/
	$(CP) \
		$(PKG_BUILD_DIR)/include \
		$(PKG_BUILD_DIR)/samples \
		$(PKG_BUILD_DIR)/scripts \
		$(PKG_BUILD_DIR)/user_headers \
		$(1)/bpf-headers
	$(CP) \
		$(CURDIR)/files/stdarg.h \
		$(1)/bpf-headers/include
endef

$(eval $(call BuildPackage,bpf-headers))

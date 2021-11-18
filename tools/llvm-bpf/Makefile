#
# Copyright (C) 2006-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=llvm-project
PKG_VERSION:=13.0.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).src.tar.xz
PKG_SOURCE_URL:=https://github.com/llvm/llvm-project/releases/download/llvmorg-$(PKG_VERSION)
PKG_HASH:=6075ad30f1ac0e15f07c1bf062c1e1268c241d674f11bd32cdf0e040c71f2bf3

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION).src

HOST_BUILD_PARALLEL:=1

CMAKE_BINARY_SUBDIR := build
CMAKE_SOURCE_SUBDIR := llvm

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

LLVM_BPF_PREFIX = llvm-bpf-$(PKG_VERSION).$(HOST_OS)-$(HOST_ARCH)

CMAKE_HOST_INSTALL_PREFIX = $(STAGING_DIR_HOST)/$(LLVM_BPF_PREFIX)

CMAKE_HOST_OPTIONS += \
	-DLLVM_ENABLE_BINDINGS=OFF \
	-DLLVM_INCLUDE_DOCS=OFF \
	-DLLVM_INCLUDE_EXAMPLES=OFF \
	-DLLVM_INCLUDE_TESTS=OFF \
	-DLLVM_ENABLE_PROJECTS="clang;lld" \
	-DLLVM_TARGETS_TO_BUILD=BPF \
	-DCLANG_BUILD_EXAMPLES=OFF \
	-DLLVM_INSTALL_TOOLCHAIN_ONLY=ON \
	-DLLVM_LINK_LLVM_DYLIB=ON \
	-DLLVM_TOOLCHAIN_TOOLS="llvm-objcopy;llvm-objdump;llvm-readelf;llvm-strip;llvm-ar;llvm-as;llvm-dis;llvm-link;llvm-nm;llvm-ranlib;llc;opt" \
	-DCMAKE_SKIP_RPATH=OFF

$(BIN_DIR)/llvm-bpf-$(PKG_VERSION).tar.xz: $(HOST_STAMP_INSTALLED)
	echo "$(PKG_VERSION)" > $(CMAKE_HOST_INSTALL_PREFIX)/.llvm-version
	STRIP_KMOD= PATCHELF= STRIP=strip $(SCRIPT_DIR)/rstrip.sh $(STAGING_DIR_HOST)/llvm-bpf
	tar -C $(STAGING_DIR_HOST) \
		-I '$(STAGING_DIR_HOST)/bin/xz -7e -T$(if $(filter 1,$(NPROC)),2,0)' \
		-cf $@.tmp llvm-bpf $(LLVM_BPF_PREFIX)
	mv $@.tmp $@

ifneq ($(CONFIG_SDK_LLVM_BPF),)
compile: $(BIN_DIR)/llvm-bpf-$(PKG_VERSION).tar.xz
endif

define Host/Install
	rm -rf $(STAGING_DIR_HOST)/llvm-bpf*
	$(Host/Install/Default)
	ln -s $(LLVM_BPF_PREFIX) $(STAGING_DIR_HOST)/llvm-bpf
endef

define Host/Uninstall
	rm -rf $(STAGING_DIR_HOST)/llvm-bpf*
endef

$(eval $(call HostBuild))

#
# Copyright (C) 2006-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=llvm-project
PKG_VERSION:=20.1.8

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).src.tar.xz
PKG_SOURCE_URL:=https://github.com/llvm/llvm-project/releases/download/llvmorg-$(PKG_VERSION)
PKG_HASH:=6898f963c8e938981e6c4a302e83ec5beb4630147c7311183cf61069af16333d
PKG_CPE_ID:=cpe:/a:llvm:llvm

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION).src

CMAKE_BINARY_SUBDIR := build
CMAKE_SOURCE_SUBDIR := llvm

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

ifneq ($(HOST_OS),Linux)
  HOST_CFLAGS := $(filter-out -I$(STAGING_DIR_HOST)/include,$(HOST_CFLAGS))
  HOST_CXXFLAGS := $(filter-out -I$(STAGING_DIR_HOST)/include,$(HOST_CXXFLAGS))
endif

LLVM_BPF_PREFIX = llvm-bpf-$(PKG_VERSION).$(HOST_OS)-$(HOST_ARCH)

CMAKE_HOST_INSTALL_PREFIX = $(STAGING_DIR_HOST)/$(LLVM_BPF_PREFIX)

CMAKE_HOST_OPTIONS += \
	-DLLVM_TARGETS_TO_BUILD=BPF \
	-DLLVM_DEFAULT_TARGET_TRIPLE=bpf \
	-DLLVM_ENABLE_PROJECTS="clang;lld" \
	-DLLVM_INSTALL_TOOLCHAIN_ONLY=ON \
	-DLLVM_LINK_LLVM_DYLIB=ON \
	-DLLVM_TOOLCHAIN_TOOLS="llvm-objcopy;llvm-objdump;llvm-readelf;llvm-strip;llvm-ar;llvm-as;llvm-dis;llvm-link;llvm-nm;llvm-ranlib;llc;opt" \
	-DLLVM_INCLUDE_BENCHMARKS=OFF \
	-DLLVM_INCLUDE_DOCS=OFF \
	-DLLVM_INCLUDE_EXAMPLES=OFF \
	-DLLVM_INCLUDE_TESTS=OFF \
	-DLLVM_ENABLE_BINDINGS=OFF \
	-DLLVM_ENABLE_CURL=OFF \
	-DLLVM_ENABLE_HTTPLIB=OFF \
	-DLLVM_ENABLE_IDE=OFF \
	-DLLVM_ENABLE_LIBEDIT=OFF \
	-DLLVM_ENABLE_LIBPFM=OFF \
	-DLLVM_ENABLE_LIBXML2=OFF \
	-DLLVM_ENABLE_OCAMLDOC=OFF \
	-DLLVM_ENABLE_Z3_SOLVER=OFF \
	-DLLVM_ENABLE_ZLIB=OFF \
	-DLLVM_ENABLE_ZSTD=OFF \
	-DLLVM_PARALLEL_LINK_JOBS=1 \
	-DCMAKE_SKIP_RPATH=OFF

define Host/Install
	rm -rf $(STAGING_DIR_HOST)/llvm-bpf*
	$(Host/Install/Default)
	ln -s $(LLVM_BPF_PREFIX) $(STAGING_DIR_HOST)/llvm-bpf
	STRIP_KMOD= PATCHELF= STRIP=strip $(SCRIPT_DIR)/rstrip.sh $(STAGING_DIR_HOST)/llvm-bpf
	echo "$(PKG_VERSION)" > $(CMAKE_HOST_INSTALL_PREFIX)/.llvm-version
endef

define Host/Uninstall
	rm -rf $(STAGING_DIR_HOST)/llvm-bpf*
endef

$(eval $(call HostBuild))

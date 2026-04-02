# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=bindgen-cli
PKG_VERSION:=0.71.1
PKG_RELEASE:=1

# Vendor tarball: upstream source + vendor/ + .cargo/config.toml.
# Generated from the upstream .crate via DownloadMethod/cargo-vendor-crate, or
# regenerate manually with: scripts/gen-cargo-vendor-tarball.sh crate bindgen-cli 0.71.1
# To update: set PKG_VENDOR_HASH:=skip and PKG_HASH:=skip, then run:
#   make tools/bindgen/download FIXUP=1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-vendor.tar.zst
PKG_SOURCE_URL:=@OPENWRT
PKG_SOURCE_PROTO:=cargo-vendor-crate
PKG_VENDOR_HASH:=skip
PKG_HASH:=9aa405a1704042a6f4c3d9184ca694701d0236bfbec363d5e78de9fe8bf1f4bb

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/rust-host-build.mk

# bindgen uses libclang to parse C/C++ headers; libclang.so comes from
# tools/llvm-bpf.  The BPF-target scope of that build is irrelevant here:
# bindgen only invokes the clang front-end, never any LLVM backend.
LIBCLANG_PATH:=$(STAGING_DIR_HOST)/llvm-bpf/lib
CARGO_HOST_VARS += LIBCLANG_PATH=$(LIBCLANG_PATH)

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_INSTALL_DIR)/bin/bindgen $(STAGING_DIR_HOST)/bin/bindgen
endef

$(eval $(call HostBuild))

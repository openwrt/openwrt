# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=bindgen-cli
PKG_VERSION:=0.73.2
PKG_RELEASE:=1

# Vendor tarball: upstream source + vendor/ + .cargo/config.toml.
# Generated from the upstream .crate via DownloadMethod/cargo-vendor-crate, or
# regenerate manually with: scripts/gen-cargo-vendor-tarball.sh crate bindgen-cli 0.73.2
# The generator makes the tarball independent of the cargo version.
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-vendor.tar.zst
PKG_SOURCE_URL:=@OPENWRT
PKG_SOURCE_PROTO:=cargo-vendor-crate
PKG_VENDOR_HASH:=5622e043625b6d1f32c84f53699690a44d1419e657879de26eb252699f5ebb7c
PKG_HASH:=344dc1b59d2cdb9c8795c5f2fad5d813dd40fc7ee725876097a3c259a06e2807

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/rust-host-build.mk

# bindgen uses libclang to parse C/C++ headers; libclang.so comes from
# tools/llvm-bpf or from the host LLVM.  The BPF-target scope of that build is
# irrelevant here: bindgen only invokes the clang front-end, never any LLVM
# backend.
CARGO_HOST_VARS += LIBCLANG_PATH=$(RUST_LIBCLANG_PATH)

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_INSTALL_DIR)/bin/bindgen $(STAGING_DIR_HOST)/bin/bindgen
endef

$(eval $(call HostBuild))

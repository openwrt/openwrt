# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=cbindgen
PKG_VERSION:=0.28.0
PKG_RELEASE:=1

# Vendor tarball: upstream source + vendor/ + .cargo/config.toml.
# Generated from the upstream .crate via DownloadMethod/cargo-vendor-crate, or
# regenerate manually with: scripts/gen-cargo-vendor-tarball.sh crate cbindgen 0.28.0
# To update: set PKG_VENDOR_HASH:=skip and PKG_HASH:=skip, then run:
#   make tools/cbindgen/download FIXUP=1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-vendor.tar.zst
PKG_SOURCE_URL:=@OPENWRT
PKG_SOURCE_PROTO:=cargo-vendor-crate
PKG_VENDOR_HASH:=skip
PKG_HASH:=3a2834f94d1ab746acbe4a51359b43de4d1cecdf23aee5d1581a1c31df6c6bcf

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/rust-host-build.mk

# cbindgen generates C headers from Rust structs (reverse direction from
# bindgen).  Required by Mesa3D Nouveau (NIL image library, NAK compiler).
# Pure Rust parser — no libclang dependency.

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_INSTALL_DIR)/bin/cbindgen $(STAGING_DIR_HOST)/bin/cbindgen
endef

$(eval $(call HostBuild))

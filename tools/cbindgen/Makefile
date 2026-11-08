# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=cbindgen
PKG_VERSION:=0.28.0
PKG_RELEASE:=1

# Vendor tarball: upstream source + vendor/ + .cargo/config.toml.
# Generated from the upstream .crate via DownloadMethod/cargo-vendor-crate, or
# regenerate manually with: scripts/gen-cargo-vendor-tarball.sh crate cbindgen 0.28.0
# The generator makes the tarball independent of the cargo version.
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-vendor.tar.zst
PKG_SOURCE_URL:=@OPENWRT
PKG_SOURCE_PROTO:=cargo-vendor-crate
PKG_VENDOR_HASH:=eadd868a2ce9ca38de7eeafdcec9c7065ef89b42b32f0839278d55f35c54d1ff
PKG_HASH:=834a03b4fcbbcbd6a2547bf15f8854b075aeb1460a6107c6db47b28a90dee15d

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/rust-host-build.mk

# cbindgen generates C headers from Rust structs (reverse direction from
# bindgen).  Required by Mesa3D Nouveau (NIL image library, NAK compiler).
# Pure Rust parser, with no libclang dependency.

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_INSTALL_DIR)/bin/cbindgen $(STAGING_DIR_HOST)/bin/cbindgen
endef

$(eval $(call HostBuild))

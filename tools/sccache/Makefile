# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=sccache
PKG_VERSION:=0.18.0
PKG_RELEASE:=1

# Vendor tarball: upstream source + vendor/ + .cargo/config.toml.
# Generated from the upstream .crate via DownloadMethod/cargo-vendor-crate, or
# regenerate manually with: scripts/gen-cargo-vendor-tarball.sh crate sccache 0.18.0
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-vendor.tar.zst
PKG_SOURCE_URL:=@OPENWRT
PKG_SOURCE_PROTO:=cargo-vendor-crate
PKG_VENDOR_HASH:=ea97b1561fe9f059860e1631e41feecfa81117bcc7bf91c895b19f24a3187538
PKG_HASH:=763980f6d9369d3f5f91d3bebdefd2da334973b0d844b5fdd769197ba50dfa5a

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/rust-host-build.mk

# sccache cannot wrap the compiler that builds it
RUST_SCCACHE_VARS:=

define Host/Compile
	$(call Host/Compile/Cargo,,--no-default-features)
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_INSTALL_DIR)/bin/sccache $(STAGING_DIR_HOST)/bin/sccache
endef

$(eval $(call HostBuild))

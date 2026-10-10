# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=corrosion
PKG_VERSION:=0.5.1
PKG_RELEASE:=1

# Vendor tarball: upstream source + generator/vendor/ + generator/.cargo/config.toml.
# Generated from the GitHub archive via DownloadMethod/cargo-vendor-github, or
# regenerate manually with:
#   scripts/gen-cargo-vendor-tarball.sh github corrosion-rs/corrosion v0.5.1 corrosion 0.5.1 generator
# The generator makes the tarball independent of the cargo version.
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-vendor.tar.zst
PKG_SOURCE_URL:=@OPENWRT
PKG_SOURCE_PROTO:=cargo-vendor-github
PKG_VENDOR_REPO:=corrosion-rs/corrosion
PKG_VENDOR_TAG:=v$(PKG_VERSION)
PKG_VENDOR_FILE:=corrosion-$(PKG_VERSION)-src.tar.gz
PKG_VENDOR_URL:=https://github.com/corrosion-rs/corrosion/archive/refs/tags
PKG_VENDOR_SUBDIR:=generator
PKG_VENDOR_HASH:=843334a9f0f5efbc225dccfa88031fe0f2ec6fd787ca1e7d55ed27b2c25d9c97
PKG_HASH:=358d3f743a3a425995b489df73405d7f7c7b72f5097c12554292b5a2613ed933

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

CMAKE_BINARY_SUBDIR:=build

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk
include $(INCLUDE_DIR)/rust-values.mk

# corrosion builds corrosion-generator (a Rust binary) during cmake configure.
# generator/.cargo/config.toml (from vendor tarball) sets net.offline=true and
# points cargo at generator/vendor/, so no network access is required.
# Export CARGO_HOME so cmake-invoked cargo uses the shared download cache.
export CARGO_HOME := $(CARGO_HOME)

CMAKE_HOST_OPTIONS += \
	-DCORROSION_BUILD_TESTS=OFF \
	-DCARGO_EXECUTABLE=$(CARGO) \
	-DRust_COMPILER=$(RUSTC)

define Host/Install
	cmake --install $(HOST_CMAKE_BINARY_DIR) --prefix $(STAGING_DIR_HOST)
endef

$(eval $(call HostBuild))

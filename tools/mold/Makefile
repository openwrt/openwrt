# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=mold
PKG_VERSION:=3.0.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-vendor.tar.zst
PKG_SOURCE_URL:=@OPENWRT
PKG_SOURCE_PROTO:=cargo-vendor-github
PKG_VENDOR_REPO:=rui314/mold
PKG_VENDOR_TAG:=v$(PKG_VERSION)
PKG_VENDOR_FILE:=v$(PKG_VERSION).tar.gz
PKG_VENDOR_SUBDIR:=cli
PKG_VENDOR_HASH:=1dee837e227b0c3f2661def602ef8ddc0b889ae5b1d28c1ddec5f29e08e5ceb5
PKG_HASH:=8a45c9eee904b9be628b0753759669e78d946250ba85bd9b4230412450f911f8

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/rust-host-build.mk

HOST_MAKE_PATH:=cli
# only enable selected targets, keep in sync with CONFIG_MOLD
CARGO_HOST_ARGS+=--no-default-features
RUST_HOST_FEATURES:=x86_64,i386,arm32,arm64,riscv64,ppc32,ppc64v1,loongarch64
# let the zstd-sys crate link against our host libzstd
CARGO_HOST_VARS+=ZSTD_SYS_USE_PKG_CONFIG=1

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_INSTALL_DIR)/bin/mold $(STAGING_DIR_HOST)/bin/
endef

$(eval $(call HostBuild))

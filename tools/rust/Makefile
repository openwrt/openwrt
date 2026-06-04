# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 Luca Barbato and Donald Hoskins

include $(TOPDIR)/rules.mk

PKG_NAME:=rust
PKG_VERSION:=1.94.0
PKG_RELEASE:=1

PKG_SOURCE:=rustc-$(PKG_VERSION)-src.tar.xz
PKG_SOURCE_URL:=https://static.rust-lang.org/dist/
PKG_HASH:=0b53ae34f5c0c3612cfe1de139f9167a018cd5737bc2205664fd69ba9b25f600

PKG_LICENSE:=Apache-2.0 MIT
PKG_LICENSE_FILES:=LICENSE-APACHE LICENSE-MIT
PKG_CPE_ID:=cpe:/a:rust-lang:rust

# Must be set BEFORE host-build.mk is included. quilt.mk does
# Host/Patch:=$(Host/Patch/Default) — an immediate := assignment that captures
# $(HOST_BUILD_DIR) at include time. Setting it first ensures the patch step
# uses the correct directory rather than the ?= default ("rust-1.94.0").
HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/rustc-$(PKG_VERSION)-src

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/rust-values.mk

# Rust-lang has an uninstall script
RUST_UNINSTALL:=$(STAGING_DIR_HOST)/lib/rustlib/uninstall.sh

HOST_CONFIGURE_VARS += CARGO_HOME="$(CARGO_HOME)"

# Build the host compiler only (no cross rust-std here; toolchain/rust handles that).
HOST_CONFIGURE_ARGS = \
	--build=$(RUSTC_HOST_ARCH) \
	--target=$(RUSTC_HOST_ARCH) \
	--host=$(RUSTC_HOST_ARCH) \
	--prefix=$(STAGING_DIR_HOST) \
	--bindir=$(STAGING_DIR_HOST)/bin \
	--libdir=$(STAGING_DIR_HOST)/lib \
	--sysconfdir=$(STAGING_DIR_HOST)/etc \
	--datadir=$(STAGING_DIR_HOST)/share \
	--mandir=$(STAGING_DIR_HOST)/man \
	--dist-compression-formats=gz \
	--disable-sanitizers \
	--release-channel=stable \
	--enable-cargo-native-static \
	--bootstrap-cache-path=$(DL_DIR)/rustc \
	"--set=llvm.targets=$(RUST_LLVM_TARGETS)" \
	"--set=llvm.experimental-targets=$(RUST_LLVM_EXPERIMENTAL_TARGETS)" \
	--set=llvm.download-ci-llvm=false

define Host/Uninstall
	[ -f $(RUST_UNINSTALL) ] && \
		$(BASH) $(RUST_UNINSTALL) || echo No Uninstall
endef

define Host/Compile
	$(RUST_SCCACHE_VARS) \
	CARGO_HOME=$(CARGO_HOME) \
	$(PYTHON) $(HOST_BUILD_DIR)/x.py \
		--build-dir $(HOST_BUILD_DIR)/build \
		dist rustc cargo llvm-tools rust-src
endef

define Host/Install
	( \
		cd $(HOST_BUILD_DIR)/build/dist ; \
		for targz in rustc-* cargo-* llvm-tools-* rust-src-*.tar.gz; do \
			[ -f "$$targz" ] || continue; \
			$(STAGING_DIR_HOST)/bin/libdeflate-gzip -dc "$$targz" | tar -xf - ; \
		done ; \
		find . -mindepth 2 -maxdepth 2 -type f -name install.sh \
			-execdir bash '{}' --prefix=$(STAGING_DIR_HOST) --disable-ldconfig \; ; \
	)
	# Install bootstrap binary so package/libs/libstd-rust can rebuild cross
	# rust-std in the SDK without needing the source build directory.
	# libstd-rust runs bootstrap directly with --keep-stage 2 so only the cross
	# std is compiled; the expensive compiler build is never repeated.
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/lib/rustlib/bootstrap
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/build/bootstrap/debug/bootstrap \
		$(STAGING_DIR_HOST)/lib/rustlib/bootstrap/
	# Install minimal source metadata into the rust-src root so that bootstrap
	# can determine channel/version and include license files in dist tarballs.
	# rust-src already provides library/ at this path; the files below are not
	# part of the rust-src component but are required by bootstrap.
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/lib/rustlib/src/rust/src/ci
	$(INSTALL_DATA) $(HOST_BUILD_DIR)/src/ci/channel \
		$(STAGING_DIR_HOST)/lib/rustlib/src/rust/src/ci/channel
	$(INSTALL_DATA) $(HOST_BUILD_DIR)/src/version \
		$(STAGING_DIR_HOST)/lib/rustlib/src/rust/src/version
	$(CP) $(HOST_BUILD_DIR)/COPYRIGHT \
		$(STAGING_DIR_HOST)/lib/rustlib/src/rust/COPYRIGHT
	$(CP) $(HOST_BUILD_DIR)/LICENSE-APACHE \
		$(STAGING_DIR_HOST)/lib/rustlib/src/rust/LICENSE-APACHE
	$(CP) $(HOST_BUILD_DIR)/LICENSE-MIT \
		$(STAGING_DIR_HOST)/lib/rustlib/src/rust/LICENSE-MIT
endef

$(eval $(call HostBuild))

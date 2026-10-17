# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 Luca Barbato and Donald Hoskins

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/rust-values.mk

PKG_NAME:=rust
PKG_VERSION:=$(RUST_VERSION)
PKG_RELEASE:=1

PKG_LICENSE:=Apache-2.0 MIT
PKG_LICENSE_FILES:=LICENSE-APACHE LICENSE-MIT
PKG_CPE_ID:=cpe:/a:rust-lang:rust

# Must be set BEFORE host-build.mk is included. quilt.mk does
# Host/Patch:=$(Host/Patch/Default) is an immediate := assignment that captures
# $(HOST_BUILD_DIR) at include time. Setting it first ensures the patch step
# uses the correct directory rather than the ?= default ("rust-1.99.0").
ifdef CONFIG_USE_RUST_BUILD
  PKG_SOURCE:=rustc-$(PKG_VERSION)-src.tar.xz
  PKG_SOURCE_URL:=https://static.rust-lang.org/dist/
  PKG_HASH:=$(RUST_SRC_TARBALL_HASH)

  HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/rustc-$(PKG_VERSION)-src
else
  HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/rust-$(PKG_VERSION)-installed
endif

include $(INCLUDE_DIR)/host-build.mk

# Rust-lang has an uninstall script
RUST_UNINSTALL:=$(STAGING_DIR_HOST)/lib/rustlib/uninstall.sh

define Host/Uninstall
	[ -f $(RUST_UNINSTALL) ] && \
		$(BASH) $(RUST_UNINSTALL) || echo No Uninstall
endef

ifdef CONFIG_USE_RUST_BUILD

HOST_CONFIGURE_VARS += CARGO_HOME="$(CARGO_HOME)"

# Only the host compiler is built here; libstd-rust builds the cross rust-std.
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

define Host/Compile
	+$(RUST_SCCACHE_VARS) \
	CARGO_HOME=$(CARGO_HOME) \
	$(PYTHON) $(HOST_BUILD_DIR)/x.py \
		--build-dir $(HOST_BUILD_DIR)/build \
		dist rustc cargo rust-std llvm-tools rust-src
endef

define Host/Install
	( \
		cd $(HOST_BUILD_DIR)/build/dist ; \
		for targz in rustc-* cargo-* rust-std-* llvm-tools-* rust-src-*.tar.gz; do \
			[ -f "$$$$targz" ] || continue; \
			$(STAGING_DIR_HOST)/bin/libdeflate-gzip -dc "$$$$targz" | tar -xf - ; \
		done ; \
		find . -mindepth 2 -maxdepth 2 -type f -name install.sh \
			-execdir bash '{}' --prefix=$(STAGING_DIR_HOST) --disable-ldconfig \; ; \
	)
endef

else

RUST_PREFIX_DIR:=$(HOST_BUILD_DIR)/prefix
RUST_UNPACK_DIR:=$(HOST_BUILD_DIR)/unpacked

$(if $(RUST_RUSTC_HASH),,$(error No prebuilt Rust components for the build host $(RUSTC_HOST_ARCH)))

define Download/rust-src
  FILE:=rust-src-$(PKG_VERSION).tar.xz
  URL:=$(RUST_DIST_URL)
  HASH:=$(RUST_SRC_HASH)
endef
$(eval $(call Download,rust-src))

define Download/rust-llvm-tools
  FILE:=llvm-tools-$(PKG_VERSION)-$(RUSTC_HOST_ARCH).tar.xz
  URL:=$(RUST_DIST_URL)
  HASH:=$(RUST_LLVM_TOOLS_HASH)
endef
$(eval $(call Download,rust-llvm-tools))

RUST_DIST_FILES:= \
	$(DL_DIR)/rust-src-$(PKG_VERSION).tar.xz \
	$(DL_DIR)/llvm-tools-$(PKG_VERSION)-$(RUSTC_HOST_ARCH).tar.xz

ifdef CONFIG_USE_RUST_PREBUILT

define Download/rustc
  FILE:=rustc-$(PKG_VERSION)-$(RUSTC_HOST_ARCH).tar.xz
  URL:=$(RUST_DIST_URL)
  HASH:=$(RUST_RUSTC_HASH)
endef
$(eval $(call Download,rustc))

define Download/cargo
  FILE:=cargo-$(PKG_VERSION)-$(RUSTC_HOST_ARCH).tar.xz
  URL:=$(RUST_DIST_URL)
  HASH:=$(RUST_CARGO_HASH)
endef
$(eval $(call Download,cargo))

define Download/rust-std
  FILE:=rust-std-$(PKG_VERSION)-$(RUSTC_HOST_ARCH).tar.xz
  URL:=$(RUST_DIST_URL)
  HASH:=$(RUST_STD_HASH)
endef
$(eval $(call Download,rust-std))

RUST_DIST_FILES+= \
	$(DL_DIR)/rustc-$(PKG_VERSION)-$(RUSTC_HOST_ARCH).tar.xz \
	$(DL_DIR)/cargo-$(PKG_VERSION)-$(RUSTC_HOST_ARCH).tar.xz \
	$(DL_DIR)/rust-std-$(PKG_VERSION)-$(RUSTC_HOST_ARCH).tar.xz

define Host/Prepare/Toolchain
endef

else

RUST_HOST_SYSROOT:=$(or $(call qstrip,$(CONFIG_RUST_TOOLCHAIN_HOST_PATH)),$(shell PATH=$(ORIG_PATH) rustc --print sysroot 2>/dev/null))

define Host/Prepare/Toolchain
	[ "$$$$($(RUST_HOST_SYSROOT)/bin/rustc --version | cut -d' ' -f2)" = "$(PKG_VERSION)" ] || { \
		echo "rustc in '$(RUST_HOST_SYSROOT)' is not version $(PKG_VERSION)"; \
		exit 1; \
	}
	$(INSTALL_DIR) $(RUST_PREFIX_DIR)/bin
	$(INSTALL_BIN) $(RUST_HOST_SYSROOT)/bin/rustc $(RUST_HOST_SYSROOT)/bin/cargo \
		$(RUST_PREFIX_DIR)/bin/
	$(CP) $(RUST_HOST_SYSROOT)/lib $(RUST_PREFIX_DIR)/
endef

define Host/Install/Check
	[ "$$$$($(STAGING_DIR_HOST)/bin/rustc --print sysroot)" = "$(shell realpath $(STAGING_DIR_HOST))" ] || { \
		echo "rustc from '$(RUST_HOST_SYSROOT)' is not relocatable, use a rustup toolchain or an unpacked official Rust distribution"; \
		exit 1; \
	}
endef

endif

define Host/Prepare
	$(INSTALL_DIR) $(RUST_PREFIX_DIR) $(RUST_UNPACK_DIR)
	$(call Host/Prepare/Toolchain)
	for tarball in $(RUST_DIST_FILES); do \
		tar -xf "$$$$tarball" -C $(RUST_UNPACK_DIR); \
	done
	for installer in $(RUST_UNPACK_DIR)/*/install.sh; do \
		$(BASH) "$$$$installer" --prefix=$(RUST_PREFIX_DIR) --disable-ldconfig; \
	done
endef

define Host/Patch
endef

define Host/Configure
endef

define Host/Compile
endef

define Host/Install
	$(CP) $(RUST_PREFIX_DIR)/. $(STAGING_DIR_HOST)/
	$(call Host/Install/Check)
endef

endif

$(eval $(call HostBuild))

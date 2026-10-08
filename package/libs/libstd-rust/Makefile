# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 Luca Barbato and Donald Hoskins

include $(TOPDIR)/rules.mk
# rust-values.mk only needs rules.mk variables, so include it before package.mk
# to make RUST_VERSION available for PKG_VERSION.
include $(INCLUDE_DIR)/rust-values.mk

PKG_NAME:=libstd-rust
PKG_VERSION:=$(RUST_VERSION)
PKG_RELEASE:=1

PKG_SOURCE:=rustc-$(RUST_VERSION)-src.tar.xz
PKG_SOURCE_URL:=https://static.rust-lang.org/dist/
PKG_HASH:=$(RUST_SRC_TARBALL_HASH)

PKG_BUILD_DIR:=$(BUILD_DIR)/rustc-$(RUST_VERSION)-src

PKG_MAINTAINER:=Daniel Golle <daniel@makrotopia.org>
PKG_LICENSE:=Apache-2.0 MIT
PKG_LICENSE_FILES:=LICENSE-APACHE LICENSE-MIT
PKG_CPE_ID:=cpe:/a:rust-lang:rust

include $(INCLUDE_DIR)/package.mk

RUSTLIB_TARGET_DIR:=$(STAGING_DIR_HOST)/lib/rustlib/$(RUSTC_TARGET_ARCH)/lib
LIBSTD_BUILD_DIR:=$(PKG_BUILD_DIR)/target/$(RUSTC_TARGET_ARCH)/release/deps

LIBSTD_FEATURES:=panic-unwind $(if $(CONFIG_DEBUG),,optimize_for_size)

LIBSTD_RUSTFLAGS:= \
	-Zforce-unstable-if-unmarked \
	-Ctarget-feature=-crt-static \
	-Ccodegen-units=1 \
	-Cembed-bitcode=yes \
	-Clink-arg=-Wl,-soname,libstd-$(LIBSTD_RUST_ID).so \
	$(if $(CONFIG_DEBUG),,-Cpanic=abort -Copt-level=z -Cstrip=debuginfo)

define Package/libstd-rust
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Rust standard library
  DEPENDS:=@USE_RUST_TOOLCHAIN $(RUST_ARCH_DEPENDS)
  ABI_VERSION:=$(LIBSTD_RUST_ID)
  URL:=https://www.rust-lang.org/
endef

define Package/libstd-rust/description
  The Rust standard library (libstd) for the target architecture.
  Required at runtime by all dynamically-linked Rust packages.
endef

define Build/Configure
endef

define Build/Compile
	+cd $(PKG_BUILD_DIR) && \
	CARGO_HOME=$(CARGO_HOME) \
	RUSTC_BOOTSTRAP=1 \
	RUSTFLAGS="$(LIBSTD_RUSTFLAGS)" \
	CARGO_TARGET_$(subst -,_,$(call toupper,$(RUSTC_TARGET_ARCH)))_LINKER=$(TARGET_CC_NOCACHE) \
	CC=$(TARGET_CC_NOCACHE) \
	TARGET_CC=$(TARGET_CC_NOCACHE) \
	TARGET_CFLAGS="$(TARGET_CFLAGS) $(RUSTC_CFLAGS)" \
	$(CARGO) build --release --locked --offline \
		--manifest-path library/sysroot/Cargo.toml \
		--target $(RUSTC_TARGET_ARCH) \
		--target-dir $(PKG_BUILD_DIR)/target \
		--features "$(LIBSTD_FEATURES)" \
		$(if $(filter --jobserver%,$(MAKEFLAGS)),,-j1)
endef

# The rlibs go to $(STAGING_DIR_HOST) because rustc is a host binary and looks
# for the target standard library in its own sysroot.
define Build/InstallDev
	rm -rf $(STAGING_DIR_HOST)/lib/rustlib/$(RUSTC_TARGET_ARCH)
	$(INSTALL_DIR) $(RUSTLIB_TARGET_DIR)
	for lib in $(LIBSTD_BUILD_DIR)/lib*.rlib $(LIBSTD_BUILD_DIR)/lib*.rmeta; do \
		case "$$$$lib" in \
		*/libsysroot-*|*/libstd.rlib) ;; \
		*) $(CP) "$$$$lib" $(RUSTLIB_TARGET_DIR)/ ;; \
		esac; \
	done
	$(INSTALL_DATA) $(LIBSTD_BUILD_DIR)/libstd.rlib \
		$(RUSTLIB_TARGET_DIR)/libstd-$(LIBSTD_RUST_ID).rlib
	$(INSTALL_DATA) $(LIBSTD_BUILD_DIR)/libstd.so \
		$(RUSTLIB_TARGET_DIR)/libstd-$(LIBSTD_RUST_ID).so
endef

define Package/libstd-rust/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(LIBSTD_BUILD_DIR)/libstd.so \
		$(1)/usr/lib/libstd-$(LIBSTD_RUST_ID).so
endef

$(eval $(call BuildPackage,libstd-rust))

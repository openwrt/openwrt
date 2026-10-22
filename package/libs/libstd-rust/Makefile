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
PKG_CONFIG_DEPENDS:=$(RUST_PKG_CONFIG_DEPENDS)

PKG_MAINTAINER:=Daniel Golle <daniel@makrotopia.org>
PKG_LICENSE:=Apache-2.0 MIT
PKG_LICENSE_FILES:=LICENSE-APACHE LICENSE-MIT
PKG_CPE_ID:=cpe:/a:rust-lang:rust

include $(INCLUDE_DIR)/package.mk

LIBSTD_TARGET_DIR:=$(PKG_BUILD_DIR)/target/$(LIBSTD_RUST_ID)
LIBSTD_BUILD_DIR:=$(LIBSTD_TARGET_DIR)/$(RUSTC_TARGET_ARCH)/release/deps

LIBSTD_RUSTFLAGS:= \
	$(RUST_LIBSTD_FLAGS) \
	-Clink-arg=-Wl,-soname,libstd-$(LIBSTD_RUST_ID).so

define Package/libstd-rust
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Rust standard library
  DEPENDS:=@USE_RUST_TOOLCHAIN $(RUST_ARCH_DEPENDS) +libgcc
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
	$(RUST_SCCACHE_VARS) \
	RUSTC=$(RUSTC) \
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
		--target-dir $(LIBSTD_TARGET_DIR) \
		--features "$(RUST_LIBSTD_FEATURES)" \
		$(if $(filter --jobserver%,$(MAKEFLAGS)),,-j1)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)$(RUSTLIB_TARGET_PATH)
	for lib in $(LIBSTD_BUILD_DIR)/lib*.rlib $(LIBSTD_BUILD_DIR)/lib*.rmeta; do \
		case "$$$$lib" in \
		*/libsysroot-*|*/libstd.rlib) ;; \
		*) $(CP) "$$$$lib" $(1)$(RUSTLIB_TARGET_PATH)/ ;; \
		esac; \
	done
	$(INSTALL_DATA) $(LIBSTD_BUILD_DIR)/libstd.rlib \
		$(1)$(RUSTLIB_TARGET_PATH)/libstd-$(LIBSTD_RUST_ID).rlib
	$(INSTALL_DATA) $(LIBSTD_BUILD_DIR)/libstd.so \
		$(1)$(RUSTLIB_TARGET_PATH)/libstd-$(LIBSTD_RUST_ID).so
endef

define Package/libstd-rust/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(LIBSTD_BUILD_DIR)/libstd.so \
		$(1)/usr/lib/libstd-$(LIBSTD_RUST_ID).so
endef

$(eval $(call BuildPackage,libstd-rust))

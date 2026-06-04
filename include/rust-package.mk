# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 Luca Barbato and Donald Hoskins

# Variables (all optional) to be set in package Makefiles:
#
# RUST_PKG_FEATURES - list of options, default empty
#
#   Space or comma separated list of features to activate
#
#   e.g. RUST_PKG_FEATURES:=enable-foo,with-bar
#
#
# RUST_PKG_LOCKED - Assert that `Cargo.lock` will remain unchanged
#                   (Enabled by default)
#
#   Disable it if you want to have up-to-date dependencies
#
#   e.g. RUST_PKG_LOCKED:=0


ifeq ($(origin RUST_INCLUDE_DIR),undefined)
  RUST_INCLUDE_DIR:=$(dir $(lastword $(MAKEFILE_LIST)))
endif
include $(RUST_INCLUDE_DIR)/rust-values.mk

RUST_PKG_LOCKED ?= 1

CARGO_PKG_VARS= \
	$(CARGO_PKG_CONFIG_VARS) \
	CC=$(HOSTCC_NOCACHE) \
	MAKEFLAGS="$(PKG_JOBS)"

CARGO_PKG_ARGS := --offline

ifeq ($(strip $(RUST_PKG_LOCKED)),1)
  CARGO_PKG_ARGS += --locked
endif

# $(1) path within PKG_BUILD_DIR (optional, defaults to MAKE_PATH)
# $(2) additional arguments to cargo (optional)
#
# All cargo builds use --offline; packages must supply a vendor tarball with
# .cargo/config.toml pointing to vendor/.  Generate vendor tarballs with
# scripts/gen-cargo-vendor-tarball.sh.
define Build/Compile/Cargo
	+$(CARGO_PKG_VARS) \
	cargo install -v \
		--profile $(CARGO_PKG_PROFILE) \
		$(if $(strip $(RUST_PKG_FEATURES)),--features "$(strip $(RUST_PKG_FEATURES))") \
		--root $(PKG_INSTALL_DIR) \
		--path "$(PKG_BUILD_DIR)/$(if $(strip $(1)),$(strip $(1)),$(strip $(MAKE_PATH)))" \
		$(if $(filter --jobserver%,$(PKG_JOBS)),,-j1) \
		$(CARGO_PKG_ARGS) \
		$(2)
endef

define RustBinPackage
  ifndef Package/$(1)/install
    define Package/$(1)/install
	$$(INSTALL_DIR) $$(1)/usr/bin/
	$$(INSTALL_BIN) $$(PKG_INSTALL_DIR)/bin/* $$(1)/usr/bin/
    endef
  endif
endef

Build/Compile=$(call Build/Compile/Cargo)

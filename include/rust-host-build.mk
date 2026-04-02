# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 Luca Barbato and Donald Hoskins

# Variables (all optional) to be set in package Makefiles:
#
# RUST_HOST_FEATURES - list of options, default empty
#
#   Space or comma separated list of features to activate
#
#   e.g. RUST_HOST_FEATURES:=enable-foo,with-bar
#
#
# RUST_HOST_LOCKED - Assert that `Cargo.lock` will remain unchanged
#                    (Enabled by default)
#
#   Disable it if you want to have up-to-date dependencies
#
#   e.g. RUST_HOST_LOCKED:=0

ifeq ($(origin RUST_INCLUDE_DIR),undefined)
  RUST_INCLUDE_DIR:=$(dir $(lastword $(MAKEFILE_LIST)))
endif
include $(RUST_INCLUDE_DIR)/rust-values.mk

RUST_HOST_LOCKED ?= 1

CARGO_HOST_VARS= \
	$(CARGO_HOST_CONFIG_VARS) \
	CC=$(HOSTCC_NOCACHE) \
	MAKEFLAGS="$(HOST_JOBS)"

CARGO_HOST_ARGS := --offline

ifeq ($(strip $(RUST_HOST_LOCKED)),1)
  CARGO_HOST_ARGS += --locked
endif

# $(1) path within HOST_BUILD_DIR (optional, defaults to HOST_MAKE_PATH)
# $(2) additional arguments to cargo (optional)
#
# All cargo builds use --offline; packages must supply a vendor tarball with
# .cargo/config.toml pointing to vendor/.  Generate vendor tarballs with
# scripts/gen-cargo-vendor-tarball.sh.
define Host/Compile/Cargo
	+$(CARGO_HOST_VARS) \
	$(CARGO) install -v \
		--profile $(CARGO_HOST_PROFILE) \
		$(if $(RUST_HOST_FEATURES),--features "$(RUST_HOST_FEATURES)") \
		--root $(HOST_INSTALL_DIR) \
		--path "$(HOST_BUILD_DIR)/$(if $(strip $(1)),$(strip $(1)),$(strip $(HOST_MAKE_PATH)))" \
		$(if $(filter --jobserver%,$(HOST_JOBS)),,-j1) \
		$(CARGO_HOST_ARGS) \
		$(2)
endef

define Host/Uninstall/Cargo
	+$(CARGO_HOST_VARS) \
	cargo uninstall -v \
		--root $(HOST_INSTALL_DIR) \
		|| true
endef

define RustBinHostBuild
  define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOSTPKG)/bin
	$(INSTALL_BIN) $(HOST_INSTALL_DIR)/bin/* $(STAGING_DIR_HOSTPKG)/bin/
  endef
endef

Host/Compile=$(call Host/Compile/Cargo)
Host/Uninstall=$(call Host/Uninstall/Cargo)

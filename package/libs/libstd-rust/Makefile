# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 Luca Barbato and Donald Hoskins

include $(TOPDIR)/rules.mk
# rust-values.mk only needs rules.mk variables, so include it before package.mk
# to make RUST_VERSION available for PKG_VERSION.
include $(INCLUDE_DIR)/rust-values.mk

PKG_NAME:=libstd-rust
PKG_RELEASE:=1

# Hash extracted from the libstd filename after Build/InstallDev runs.
# Lazy (=) so it is re-evaluated when the ipk control file is generated,
# at which point Build/InstallDev has already written the hash file.
LIBSTD_HASH = $(shell \
	cat $(STAGING_DIR_HOST)/lib/rustlib/$(RUSTC_TARGET_ARCH)/lib/.libstd-hash \
	2>/dev/null)
PKG_VERSION = $(RUST_VERSION)$(if $(LIBSTD_HASH),+$(LIBSTD_HASH))

PKG_LICENSE:=Apache-2.0 MIT
PKG_LICENSE_FILES:=LICENSE-APACHE LICENSE-MIT
PKG_CPE_ID:=cpe:/a:rust-lang:rust

include $(INCLUDE_DIR)/package.mk

# Bootstrap binary installed by tools/rust.  Invoked directly (bypassing
# x.py and the Python bootstrap infrastructure) so this package can be built
# from the OpenWrt SDK without access to the tools/ build directory.
RUST_BOOTSTRAP_BIN := $(STAGING_DIR_HOST)/lib/rustlib/bootstrap/bootstrap

# Source root used with --src: rust-src installs library/ here, and tools/rust
# additionally installs src/ci/channel, src/version, and license files so that
# bootstrap can determine the channel/version and populate dist tarballs.
RUST_SRC_ROOT := $(STAGING_DIR_HOST)/lib/rustlib/src/rust

define Package/libstd-rust
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Rust standard library
  # @USE_RUST_TOOLCHAIN ensures tools/rust (which provides the bootstrap binary
  # and installs the compiler to staging_dir) is built before any package runs.
  # tools/ host builds are not packages, so PKG_BUILD_DEPENDS cannot reference
  # them; the @SYMBOL dependency + $(tools/stamp-compile) prerequisite is the
  # correct mechanism.
  DEPENDS:=@USE_RUST_TOOLCHAIN $(RUST_ARCH_DEPENDS)
  URL:=https://www.rust-lang.org/
endef

define Package/libstd-rust/description
  The Rust standard library (libstd) for the target architecture.
  Required at runtime by all dynamically-linked Rust packages.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/build/$(RUSTC_HOST_ARCH)
	# Create a synthetic stage2 sysroot pointing to the installed compiler.
	# bootstrap --keep-stage 2 reads the compiler from here instead of
	# rebuilding it.  All new artifacts (cross rust-std, dist tarballs) are
	# written to PKG_BUILD_DIR and never pollute staging_dir.
	# The installed rustc in staging_dir IS the stage2 compiler; the directory
	# layout matches exactly what bootstrap expects under stage2/.
	ln -sf $(STAGING_DIR_HOST) \
		$(PKG_BUILD_DIR)/build/$(RUSTC_HOST_ARCH)/stage2
endef

# Write a bootstrap.toml for the bootstrap binary.
# build-dir is set here (not on the CLI) so the isolated PKG_BUILD_DIR is
# used for all output.  --src on the CLI points to the installed rust-src root.
define Build/Configure
	( \
		printf '[build]\n'; \
		printf 'build = "%s"\n' '$(RUSTC_HOST_ARCH)'; \
		printf 'host = ["%s"]\n' '$(RUSTC_HOST_ARCH)'; \
		printf 'target = ["%s", "%s"]\n' '$(RUSTC_TARGET_ARCH)' '$(RUSTC_HOST_ARCH)'; \
		printf 'build-dir = "%s/build"\n' '$(PKG_BUILD_DIR)'; \
		printf 'bootstrap-cache-path = "%s"\n' '$(DL_DIR)/rustc'; \
		printf 'cargo-native-static = true\n'; \
		printf 'sanitizers = false\n'; \
		printf '\n[install]\n'; \
		printf 'prefix = "%s"\n' '$(STAGING_DIR_HOST)'; \
		printf 'bindir = "%s/bin"\n' '$(STAGING_DIR_HOST)'; \
		printf 'libdir = "%s/lib"\n' '$(STAGING_DIR_HOST)'; \
		printf 'sysconfdir = "%s/etc"\n' '$(STAGING_DIR_HOST)'; \
		printf 'datadir = "%s/share"\n' '$(STAGING_DIR_HOST)'; \
		printf 'mandir = "%s/man"\n' '$(STAGING_DIR_HOST)'; \
		printf '\n[llvm]\n'; \
		printf 'download-ci-llvm = false\n'; \
		printf '\n[rust]\n'; \
		printf 'channel = "stable"\n'; \
		printf 'codegen-units-std = 1\n'; \
		if [ '$(CONFIG_DEBUG)' != 'y' ]; then \
			printf 'debuginfo-level-std = 0\n'; \
			printf 'strip = true\n'; \
			printf 'backtrace = false\n'; \
			printf 'std-features = ["optimize_for_size"]\n'; \
		fi; \
		printf '\n[dist]\n'; \
		printf 'compression-formats = ["gz"]\n'; \
		printf '\n[target.%s]\n' '$(RUSTC_TARGET_ARCH)'; \
		printf 'ar = "%s"\n' '$(TARGET_AR)'; \
		printf 'cc = "%s"\n' '$(TARGET_CC_NOCACHE)'; \
		printf 'cxx = "%s"\n' '$(TARGET_CXX_NOCACHE)'; \
		printf 'linker = "%s"\n' '$(TARGET_CC_NOCACHE)'; \
		printf 'ranlib = "%s"\n' '$(TARGET_RANLIB)'; \
		printf 'crt-static = false\n'; \
		$(if $(CONFIG_USE_MUSL),printf 'musl-root = "%s"\n' '$(TOOLCHAIN_ROOT_DIR)';) \
		if [ '$(CONFIG_DEBUG)' = 'y' ]; then \
			printf 'rustflags = ["-C", "panic=unwind"]\n'; \
		else \
			printf 'rustflags = ["-C", "panic=abort", "-C", "opt-level=z"]\n'; \
		fi; \
	) > $(PKG_BUILD_DIR)/bootstrap.toml
endef

define Build/Compile
	CARGO_HOME=$(CARGO_HOME) \
	TARGET_CFLAGS="$(TARGET_CFLAGS) $(RUSTC_CFLAGS)" \
	$(RUST_BOOTSTRAP_BIN) \
		--config $(PKG_BUILD_DIR)/bootstrap.toml \
		--src $(RUST_SRC_ROOT) \
		--keep-stage 2 \
		dist rust-std
endef

# Install rlibs into $(STAGING_DIR_HOST)/lib/rustlib/$(RUSTC_TARGET_ARCH)/lib/
# so that the host-side rustc binary can find them when cross-compiling packages.
# After installation, extract the metadata hash from the libstd filename and record
# it in .libstd-hash.  The $(eval) also updates LIBSTD_HASH for the current Make
# session so that PKG_VERSION is correct when the ipk control file is written.
define Build/InstallDev
	( \
		cd $(PKG_BUILD_DIR)/build/dist ; \
		for targz in rust-std-*-$(RUSTC_TARGET_ARCH).tar.gz; do \
			[ -f "$$targz" ] || continue; \
			$(STAGING_DIR_HOST)/bin/libdeflate-gzip -dc "$$targz" | tar -xf - ; \
		done ; \
		find . -mindepth 2 -maxdepth 2 -type f -name install.sh \
			-execdir bash '{}' --prefix=$(STAGING_DIR_HOST) --disable-ldconfig \; ; \
	)
	$(eval LIBSTD_HASH := $(shell \
		find $(STAGING_DIR_HOST)/lib/rustlib/$(RUSTC_TARGET_ARCH)/lib/ \
			-name 'libstd-*.so' -maxdepth 1 2>/dev/null | \
		head -1 | \
		sed 's/.*libstd-\(.*\)\.so$$/\1/'))
	echo "$(LIBSTD_HASH)" \
		> $(STAGING_DIR_HOST)/lib/rustlib/$(RUSTC_TARGET_ARCH)/lib/.libstd-hash
endef

# Ship libstd-HASH.so (and libpanic_unwind-HASH.so for debug builds) on-device.
# All cargo-built packages compiled with -C prefer-dynamic need this at runtime.
define Package/libstd-rust/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(STAGING_DIR_HOST)/lib/rustlib/$(RUSTC_TARGET_ARCH)/lib/libstd-*.so \
		$(1)/usr/lib/
	$(if $(CONFIG_DEBUG), \
		$(CP) $(STAGING_DIR_HOST)/lib/rustlib/$(RUSTC_TARGET_ARCH)/lib/libpanic_unwind-*.so \
			$(1)/usr/lib/)
endef

$(eval $(call BuildPackage,libstd-rust))

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

ifdef CONFIG_USE_RUST_BUILD
  PKG_SOURCE:=rustc-$(PKG_VERSION)-src.tar.xz
  PKG_SOURCE_URL:=https://static.rust-lang.org/dist/
  PKG_HASH:=$(RUST_SRC_TARBALL_HASH)
endif

# All modes share one build directory, so that a mode change prepares anew.
HOST_PREPARED_DEPENDS:= \
	CONFIG_USE_RUST_BUILD CONFIG_USE_RUST_PREBUILT CONFIG_USE_RUST_HOST \
	CONFIG_RUST_TOOLCHAIN_HOST_PATH

include $(INCLUDE_DIR)/host-build.mk

TAR_OPTIONS+= --strip-components 1
TAR_CMD=$(HOST_TAR) -C $(1) $(TAR_OPTIONS)

RUST_PREFIX_DIR:=$(HOST_BUILD_DIR)/prefix
RUST_FILE_LIST:=$(STAGING_DIR_HOST)/stamp/.rust_files
RUST_DIR_LIST:=$(STAGING_DIR_HOST)/stamp/.rust_dirs

# The rust-installer manifests name absolute paths, which its uninstaller
# and the next install.sh into the same prefix delete.
RUST_INSTALLER_DATA:=$(addprefix $(RUST_PREFIX_DIR)/lib/rustlib/, \
	components install.log uninstall.sh rust-installer-version manifest-*)

define Host/Install/RemoveInstallerData
	rm -f $(RUST_INSTALLER_DATA)
endef

# Cargo reads .cargo/config.toml in every parent of its working directory.
define Host/CheckCargoConfig
	dir="$$$$(cd $(TOPDIR) && pwd -P)"; \
	while :; do \
		for file in "$$$$dir/.cargo/config" "$$$$dir/.cargo/config.toml"; do \
			[ ! -e "$$$$file" ] || { echo "Cargo would read $$$$file"; exit 1; }; \
		done; \
		[ "$$$$dir" != / ] || break; \
		dir="$$$$(dirname "$$$$dir")"; \
	done
endef
Hooks/HostPrepare/Pre += Host/CheckCargoConfig

define Host/Uninstall
	if [ -f $(RUST_FILE_LIST) ]; then \
		cd $(STAGING_DIR_HOST) && \
		while read -r file; do rm -f "$$$$file"; done < $(RUST_FILE_LIST) && \
		while read -r dir; do rmdir "$$$$dir" 2>/dev/null || true; done < $(RUST_DIR_LIST); \
	fi
	rm -f $(RUST_FILE_LIST) $(RUST_DIR_LIST)
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

# bootstrap leaves the job count of ninja and cargo to the jobserver named in
# MAKEFLAGS, and runs one job per CPU without one.
define Host/Compile
	+CARGO_HOME=$(CARGO_HOME) \
	MAKEFLAGS="$(MAKE_JOBSERVER)" \
	$(PYTHON) $(HOST_BUILD_DIR)/x.py \
		--build-dir $(HOST_BUILD_DIR)/build \
		$(if $(MAKE_JOBSERVER),,--jobs 1) \
		dist rustc cargo rust-std llvm-tools rust-src
endef

define Host/Install/Prefix
	rm -rf $(RUST_PREFIX_DIR)
	( \
		cd $(HOST_BUILD_DIR)/build/dist ; \
		for targz in rustc-* cargo-* rust-std-* llvm-tools-* rust-src-*.tar.gz; do \
			[ -f "$$$$targz" ] || continue; \
			$(STAGING_DIR_HOST)/bin/libdeflate-gzip -dc "$$$$targz" | tar -xf - ; \
		done ; \
		find . -mindepth 2 -maxdepth 2 -type f -name install.sh \
			-execdir bash '{}' --prefix=$(RUST_PREFIX_DIR) --disable-ldconfig \; ; \
	)
endef

else

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

# install.sh renames every file it finds in the prefix to *.old, so the host
# toolchain is copied over the installed components.
define Host/Prepare
	$(INSTALL_DIR) $(RUST_PREFIX_DIR) $(RUST_UNPACK_DIR)
	for tarball in $(RUST_DIST_FILES); do \
		tar -xf "$$$$tarball" -C $(RUST_UNPACK_DIR); \
	done
	for installer in $(RUST_UNPACK_DIR)/*/install.sh; do \
		$(BASH) "$$$$installer" --prefix=$(RUST_PREFIX_DIR) --disable-ldconfig; \
	done
	$(call Host/Prepare/Toolchain)
endef

define Host/Patch
endef

define Host/Configure
endef

define Host/Compile
endef

endif

define Host/Install
	$(call Host/Install/Prefix)
	$(call Host/Install/RemoveInstallerData)
	$(call Host/Uninstall)
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/stamp
	cd $(RUST_PREFIX_DIR) && \
		find . ! -type d > $(RUST_FILE_LIST) && \
		find . -mindepth 1 -depth -type d > $(RUST_DIR_LIST)
	$(CP) $(RUST_PREFIX_DIR)/. $(STAGING_DIR_HOST)/
	$(call Host/Install/Check)
endef

$(eval $(call HostBuild))

# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 Luca Barbato and Donald Hoskins

# Single source of truth for version, used by toolchain/rust and toolchain/bindgen
RUST_VERSION:=1.94.0

# Paths to host Rust toolchain binaries
RUSTC:=$(STAGING_DIR_HOST)/bin/rustc
CARGO:=$(STAGING_DIR_HOST)/bin/cargo

# sccache wrapper — set by CONFIG_RUST_SCCACHE in toolchain/Config.in.
# When enabled, passes RUSTC_WRAPPER=sccache (and optionally SCCACHE_DIR) to
# any x.py or cargo invocation that builds the compiler or packages.
ifdef CONFIG_RUST_SCCACHE
  RUST_SCCACHE_VARS = \
	RUSTC_WRAPPER=sccache \
	$(if $(CONFIG_RUST_SCCACHE_DIR),SCCACHE_DIR=$(CONFIG_RUST_SCCACHE_DIR))
endif

# Source directory reused by toolchain/rust build.
# tools/rust lives in tools/ context where BUILD_DIR_HOST = build_dir/host (not
# build_dir/hostpkg as in package/ context); use BUILD_DIR_BASE/host explicitly
# so this resolves to the same path regardless of which context includes this file.
RUST_SRC_DIR:=$(BUILD_DIR_BASE)/host/rustc-$(RUST_VERSION)-src

# Rust Environmental Vars
RUSTC_HOST_SUFFIX:=$(word 4, $(subst -, ,$(GNU_HOST_NAME)))
RUSTC_HOST_ARCH:=$(HOST_ARCH)-unknown-linux-$(RUSTC_HOST_SUFFIX)
CARGO_HOME:=$(DL_DIR)/cargo

ifeq ($(CONFIG_USE_MUSL),y)
  # Force linking of the SSP library for musl
  ifdef CONFIG_PKG_CC_STACKPROTECTOR_REGULAR
    ifeq ($(strip $(PKG_SSP)),1)
      RUSTC_LDFLAGS+=-lssp_nonshared
    endif
  endif
  ifdef CONFIG_PKG_CC_STACKPROTECTOR_STRONG
    ifeq ($(strip $(PKG_SSP)),1)
      RUSTC_LDFLAGS+=-lssp_nonshared
    endif
  endif
endif

CARGO_RUSTFLAGS+=-Ctarget-feature=-crt-static $(RUSTC_LDFLAGS)

# Dynamically link against libstd.so instead of bundling it statically.
# libstd-HASH.so is packaged as package/libs/libstd-rust and ships on-device.
# Only affects target (PKG) builds via CARGO_PKG_CONFIG_VARS/RUSTFLAGS;
# host tools (bindgen, cbindgen) use CARGO_HOST_CONFIG_VARS which omits RUSTFLAGS.
CARGO_RUSTFLAGS+=-C prefer-dynamic

ifeq ($(HOST_OS),Darwin)
  ifeq ($(HOST_ARCH),arm64)
    RUSTC_HOST_ARCH:=aarch64-apple-darwin
  else ifeq ($(HOST_ARCH),x86_64)
    RUSTC_HOST_ARCH:=x86_64-apple-darwin
  endif
endif

# mips64 n64 ABI → muslabi64 suffix; n32 has no Rust target; o32 not standard on 64-bit
ifeq ($(ARCH),mips64)
  RUSTC_TARGET_ARCH:=$(subst openwrt,unknown,$(REAL_GNU_TARGET_NAME))
  ifeq ($(CONFIG_MIPS64_ABI_N64),y)
    RUSTC_TARGET_ARCH:=$(subst musl,muslabi64,$(RUSTC_TARGET_ARCH))
  endif
else ifeq ($(ARCH),mips64el)
  RUSTC_TARGET_ARCH:=$(subst openwrt,unknown,$(REAL_GNU_TARGET_NAME))
  ifeq ($(CONFIG_MIPS64_ABI_N64),y)
    RUSTC_TARGET_ARCH:=$(subst musl,muslabi64,$(RUSTC_TARGET_ARCH))
  endif
else
  RUSTC_TARGET_ARCH:=$(subst openwrt,unknown,$(REAL_GNU_TARGET_NAME))
endif

RUSTC_TARGET_ARCH:=$(subst muslgnueabi,musleabi,$(RUSTC_TARGET_ARCH))

ifeq ($(ARCH),i386)
  RUSTC_TARGET_ARCH:=$(subst i486,i586,$(RUSTC_TARGET_ARCH))
else ifeq ($(ARCH),riscv64)
  RUSTC_TARGET_ARCH:=$(subst riscv64,riscv64gc,$(RUSTC_TARGET_ARCH))
endif

# ARM Logic
ifeq ($(ARCH),arm)
  ifeq ($(CONFIG_arm_v6)$(CONFIG_arm_v7),)
    RUSTC_TARGET_ARCH:=$(subst arm,armv5te,$(RUSTC_TARGET_ARCH))
  else ifeq ($(CONFIG_arm_v7),y)
    RUSTC_TARGET_ARCH:=$(subst arm,armv7,$(RUSTC_TARGET_ARCH))
  endif

  ifeq ($(CONFIG_HAS_FPU),y)
    RUSTC_TARGET_ARCH:=$(subst musleabi,musleabihf,$(RUSTC_TARGET_ARCH))
    RUSTC_TARGET_ARCH:=$(subst gnueabi,gnueabihf,$(RUSTC_TARGET_ARCH))
  endif
endif

ifeq ($(ARCH),aarch64)
    RUSTC_CFLAGS:=-mno-outline-atomics
endif

# Support only architectures with a valid Rust target.
# powerpc/powerpc64 with SPE FPU append 'spe' to TARGET_SUFFIX → invalid target.
# mips64/mips64el n32 ABI has no standard Rust target.
RUST_ARCH_DEPENDS:=@(aarch64||arm||i386||loongarch64||mips||(!MIPS64_ABI_N32&&mips64)||(!MIPS64_ABI_N32&&mips64el)||mipsel||(!HAS_SPE_FPU&&powerpc)||(!HAS_SPE_FPU&&powerpc64)||riscv64||x86_64)

# LLVM codegen backends to compile into rustc — one per unique LLVM target name
# covering all architectures in RUST_ARCH_DEPENDS (semicolon-separated):
#   aarch64, arm           → AArch64, ARM
#   i386, x86_64           → X86
#   loongarch64            → LoongArch
#   mips, mipsel,
#     mips64, mips64el     → Mips
#   powerpc, powerpc64     → PowerPC
#   riscv64                → RISCV
RUST_LLVM_TARGETS:=AArch64;ARM;LoongArch;Mips;PowerPC;RISCV;X86

# Experimental LLVM backends (bootstrap key: llvm.experimental-targets).
# Defaults to AVR;M68k;CSKY;Xtensa in Rust 1.94 — none relevant to OpenWrt.
RUST_LLVM_EXPERIMENTAL_TARGETS:=

# Hash of the cross rust-std, recorded by package/libs/libstd-rust/Build/InstallDev.
# Empty before libstd-rust has been built (clean build / dump phase).
# Lazy (=): the shell is re-evaluated at each reference.  By the time any cargo
# package is compiled, libstd-rust is already built (PKG_BUILD_DEPENDS:=libstd-rust)
# and the hash file exists, so DEPENDS in the resulting ipk carries the exact version.
LIBSTD_RUST_HASH = $(shell \
	cat $(STAGING_DIR_HOST)/lib/rustlib/$(RUSTC_TARGET_ARCH)/lib/.libstd-hash \
	2>/dev/null)

# RUST_PKG_DEPENDS: use in Package/foo DEPENDS for all cargo-built target packages.
# Includes arch constraint, toolchain config guard, and a versioned dep on the exact
# libstd-rust that was built.
# Lazy (=) so the hash is expanded at the time the dependent package's ipk is created.
RUST_PKG_DEPENDS = @USE_RUST_TOOLCHAIN $(RUST_ARCH_DEPENDS) \
	+libstd-rust$(if $(LIBSTD_RUST_HASH),=$(RUST_VERSION)+$(LIBSTD_RUST_HASH))

CARGO_HOST_CONFIG_VARS= \
	CARGO_HOME=$(CARGO_HOME)

CARGO_HOST_PROFILE:=release

CARGO_PKG_CONFIG_VARS= \
	CARGO_BUILD_TARGET=$(RUSTC_TARGET_ARCH) \
	CARGO_HOME=$(CARGO_HOME) \
	CARGO_PROFILE_RELEASE_CODEGEN_UNITS=1 \
	CARGO_PROFILE_RELEASE_DEBUG=false \
	CARGO_PROFILE_RELEASE_DEBUG_ASSERTIONS=false \
	CARGO_PROFILE_RELEASE_LTO=true \
	CARGO_PROFILE_RELEASE_OPT_LEVEL=z \
	CARGO_PROFILE_RELEASE_OVERFLOW_CHECKS=true \
	CARGO_PROFILE_RELEASE_PANIC=$(if $(CONFIG_DEBUG),unwind,abort) \
	CARGO_PROFILE_RELEASE_RPATH=false \
	CARGO_TARGET_$(subst -,_,$(call toupper,$(RUSTC_TARGET_ARCH)))_LINKER=$(TARGET_CC_NOCACHE) \
	RUSTFLAGS="$(CARGO_RUSTFLAGS)" \
	TARGET_CC=$(TARGET_CC_NOCACHE) \
	TARGET_CFLAGS="$(TARGET_CFLAGS) $(RUSTC_CFLAGS)"

CARGO_PKG_PROFILE:=$(if $(CONFIG_DEBUG),dev,release)

CARGO_RUSTFLAGS+=-Clink-arg=-fuse-ld=$(TARGET_LINKER)

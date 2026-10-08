# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 Luca Barbato and Donald Hoskins

ifneq ($(__inc_rust_values),1)
__inc_rust_values:=1

# Single source of truth for the Rust version, used by tools/rust and
# package/libs/libstd-rust
RUST_VERSION:=1.99.0
RUST_SRC_TARBALL_HASH:=cc41916a8c84f5d9ec4f55561b44e39f43b647b133f8a6f51be9ea13c83f7036

# Paths to host Rust toolchain binaries
RUSTC:=$(STAGING_DIR_HOST)/bin/rustc
CARGO:=$(STAGING_DIR_HOST)/bin/cargo

# Directory holding libclang for bindgen: the host LLVM when selected as BPF
# toolchain, otherwise the one built by tools/llvm-bpf.
ifdef CONFIG_USE_LLVM_HOST
  RUST_HOST_LLVM_PREFIX:=$(call qstrip,$(CONFIG_BPF_TOOLCHAIN_HOST_PATH))
  RUST_HOST_LLVM_LIBDIRS:= \
	$(if $(RUST_HOST_LLVM_PREFIX),$(RUST_HOST_LLVM_PREFIX)/lib $(RUST_HOST_LLVM_PREFIX)/lib64) \
	$(shell PATH='$(if $(RUST_HOST_LLVM_PREFIX),$(RUST_HOST_LLVM_PREFIX)/bin:)$(PATH)' \
		llvm-config --libdir 2>/dev/null) \
	/usr/lib /usr/lib64 /usr/local/lib
  RUST_LIBCLANG_PATH:=$(firstword $(foreach d,$(RUST_HOST_LLVM_LIBDIRS), \
	$(if $(wildcard $(d)/libclang.so* $(d)/libclang-*.so* $(d)/libclang.dylib),$(d))))
else
  RUST_LIBCLANG_PATH:=$(STAGING_DIR_HOST)/llvm-bpf/lib
endif

# sccache wrapper, set by CONFIG_RUST_SCCACHE in toolchain/Config.in.
# When enabled, passes RUSTC_WRAPPER=sccache (and optionally SCCACHE_DIR) to
# any x.py or cargo invocation that builds the compiler or packages.
ifdef CONFIG_RUST_SCCACHE
  RUST_SCCACHE_VARS = \
	RUSTC_WRAPPER=sccache \
	$(if $(CONFIG_RUST_SCCACHE_DIR),SCCACHE_DIR=$(CONFIG_RUST_SCCACHE_DIR))
endif

# Source directory of the tools/rust build.
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

CARGO_RUSTFLAGS+=-Ctarget-feature=-crt-static $(RUSTC_LDFLAGS) $(if $(CONFIG_DEBUG),,-Cpanic=abort)

ifeq ($(HOST_OS),Darwin)
  ifeq ($(HOST_ARCH),arm64)
    RUSTC_HOST_ARCH:=aarch64-apple-darwin
  else ifeq ($(HOST_ARCH),x86_64)
    RUSTC_HOST_ARCH:=x86_64-apple-darwin
  endif
endif

# Prebuilt Rust components of the release, pinned by hash. They are the
# compiler of the prebuilt choice, rust-src and llvm-tools of the prebuilt and
# host choices, and the cargo that generates vendor tarballs without one.
RUST_DIST_URL:=https://static.rust-lang.org/dist/2026-10-01/
RUST_SRC_HASH:=3f1f9b7ed48f4596fc87889b7b3c61747336a55c9c22db1ab0c697e0aadb77aa
ifeq ($(RUSTC_HOST_ARCH),x86_64-unknown-linux-gnu)
  RUST_RUSTC_HASH:=77171ba2a0345fdf2abc4fedda55d6de078dae7a68527c28be8c77dcc9604bd5
  RUST_CARGO_HASH:=d7674918d28093097614cd9728b6ca60db9ea3038f640f0bd1e9a4188c7568ce
  RUST_STD_HASH:=3e58dff2d0b72196b5ea4e90536e174d400de88564a52694686b81e091169933
  RUST_LLVM_TOOLS_HASH:=91a6db5668c08ee68e330e5b0fda27c11b79b90592627e89f7c5db12af2ff0ba
else ifeq ($(RUSTC_HOST_ARCH),aarch64-unknown-linux-gnu)
  RUST_RUSTC_HASH:=89c0f1a3a44df63c95e5d5f2ba093d0a12c5f5f57f8635f8a7a0ce50cf86600e
  RUST_CARGO_HASH:=113229fcc16cc1ba004923a4cc275fbf3281dafaaf8b97269f9c00189d497fd4
  RUST_STD_HASH:=1cf7e2ef58ed1cfa6f1adea18e155cbf254c1951f39e5f5e3376879f55fd64a4
  RUST_LLVM_TOOLS_HASH:=6e921c7c1225ec43dcdd0bda2ce541ac79d45eaa2da64c686f7d78e8cb3dff04
else ifeq ($(RUSTC_HOST_ARCH),x86_64-apple-darwin)
  RUST_RUSTC_HASH:=7460910254f059b49b99ee3ddbcf7197d16b9505b59c166e14a1465ee323a81d
  RUST_CARGO_HASH:=939b71952afd63ae73bc00f3517c19f0dd7ac54884b5fac16a813ee50985d14d
  RUST_STD_HASH:=18ee81961a73c3ae966ecb1e8f6096438c90aeefece8856201a6ecc17f5bc850
  RUST_LLVM_TOOLS_HASH:=708b998c88ccbf2ce847cc8aeeb9fa55b969319848971a9e2085c8e426b907cf
else ifeq ($(RUSTC_HOST_ARCH),aarch64-apple-darwin)
  RUST_RUSTC_HASH:=334a66714ca316d71bbe5efe44f71762d0b276cea73be2987374693a837f7450
  RUST_CARGO_HASH:=76abff0ad79a10a3152dff640ea0d8b3c37637d9300bf55da41b95c3ddc069c1
  RUST_STD_HASH:=a7c6de8aa21e7c31163a7656295b321bb85f5dce55ed8ea95ac9ac561202bb5b
  RUST_LLVM_TOOLS_HASH:=9cbaa83827e53c9bbae05fa3a36aff0832448de6d2204829b710cbc85567ad2d
endif
RUST_CARGO_URL:=$(RUST_DIST_URL)cargo-$(RUST_VERSION)-$(RUSTC_HOST_ARCH).tar.xz

# mips64 n64 ABI uses the muslabi64 suffix; n32 has no Rust target; o32 is not standard on 64-bit
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
# powerpc/powerpc64 with SPE FPU append 'spe' to TARGET_SUFFIX, which is an invalid target.
# mips64/mips64el n32 ABI has no standard Rust target.
RUST_ARCH_DEPENDS:=@(aarch64||arm||i386||loongarch64||mips||(!MIPS64_ABI_N32&&mips64)||(!MIPS64_ABI_N32&&mips64el)||mipsel||(!HAS_SPE_FPU&&powerpc)||(!HAS_SPE_FPU&&powerpc64)||riscv64||x86_64)

# LLVM codegen backends to compile into rustc, one per unique LLVM target name
# covering all architectures in RUST_ARCH_DEPENDS (semicolon-separated):
#   aarch64, arm           : AArch64, ARM
#   i386, x86_64           : X86
#   loongarch64            : LoongArch
#   mips, mipsel,
#     mips64, mips64el     : Mips
#   powerpc, powerpc64     : PowerPC
#   riscv64                : RISCV
RUST_LLVM_TARGETS:=AArch64;ARM;LoongArch;Mips;PowerPC;RISCV;X86

# Experimental LLVM backends (bootstrap key: llvm.experimental-targets).
# Defaults to AVR;M68k;CSKY;Xtensa, none of which are relevant to OpenWrt.
RUST_LLVM_EXPERIMENTAL_TARGETS:=

# Link against libstd-HASH.so (shipped by package/libs/libstd-rust) instead of
# bundling libstd.  rustc rejects -C prefer-dynamic together with LTO, so LTO
# is switched off for packages that link dynamically.  Override per package
# with RUST_PREFER_DYNAMIC:=0.
RUST_PREFER_DYNAMIC ?= 1

# Identifier in the name of the shared libstd, libstd-ID.so, and ABI version
# of package/libs/libstd-rust. It changes with every input that changes the
# shared library.
LIBSTD_RUST_ID = $(shell printf '%08x' "$$(printf '%s' '$(RUST_VERSION) $(RUSTC_TARGET_ARCH) $(if $(CONFIG_DEBUG),debug,release)' | cksum | cut -d' ' -f1)")

# RUST_PKG_DEPENDS: use in Package/foo DEPENDS for all cargo-built target packages.
# Includes arch constraint, toolchain config guard, and the libstd-rust
# runtime dependency when the package links libstd dynamically.
RUST_PKG_DEPENDS = @USE_RUST_TOOLCHAIN $(RUST_ARCH_DEPENDS) \
	$(if $(filter 1,$(RUST_PREFER_DYNAMIC)),+libstd-rust)

CARGO_HOST_CONFIG_VARS= \
	CARGO_HOME=$(CARGO_HOME)

CARGO_HOST_PROFILE:=release

CARGO_PKG_CONFIG_VARS= \
	CARGO_BUILD_TARGET=$(RUSTC_TARGET_ARCH) \
	CARGO_HOME=$(CARGO_HOME) \
	CARGO_PROFILE_RELEASE_CODEGEN_UNITS=1 \
	CARGO_PROFILE_RELEASE_DEBUG=false \
	CARGO_PROFILE_RELEASE_DEBUG_ASSERTIONS=false \
	CARGO_PROFILE_RELEASE_LTO=$(if $(filter 1,$(RUST_PREFER_DYNAMIC)),false,true) \
	CARGO_PROFILE_RELEASE_OPT_LEVEL=z \
	CARGO_PROFILE_RELEASE_OVERFLOW_CHECKS=true \
	CARGO_PROFILE_RELEASE_PANIC=$(if $(CONFIG_DEBUG),unwind,abort) \
	CARGO_PROFILE_RELEASE_RPATH=false \
	CARGO_TARGET_$(subst -,_,$(call toupper,$(RUSTC_TARGET_ARCH)))_LINKER=$(TARGET_CC_NOCACHE) \
	RUSTFLAGS="$(CARGO_RUSTFLAGS) $(if $(filter 1,$(RUST_PREFER_DYNAMIC)),-C prefer-dynamic)" \
	TARGET_CC=$(TARGET_CC_NOCACHE) \
	TARGET_CFLAGS="$(TARGET_CFLAGS) $(RUSTC_CFLAGS)"

CARGO_PKG_PROFILE:=$(if $(CONFIG_DEBUG),dev,release)

CARGO_RUSTFLAGS+=-Clink-arg=-fuse-ld=$(TARGET_LINKER)

endif # __inc_rust_values

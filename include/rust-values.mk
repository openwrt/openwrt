# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 Luca Barbato and Donald Hoskins

ifneq ($(__inc_rust_values),1)
__inc_rust_values:=1

# Single source of truth for the Rust version
RUST_VERSION:=1.99.1
RUST_SRC_TARBALL_HASH:=6451598a99e77f07571960f6b21d577399d3a30f33e80cbd2e3a4aae6535d265

# Paths to host Rust toolchain binaries. Without a toolchain of its own, the
# build uses rust/host of the packages feed, installed in $(STAGING_DIR)/host.
ifdef CONFIG_USE_RUST_TOOLCHAIN
  RUSTC:=$(STAGING_DIR_HOST)/bin/rustc
  CARGO:=$(STAGING_DIR_HOST)/bin/cargo
  RUSTC_ID:=$(shell $(RUSTC) --version --verbose 2>/dev/null)
else
  RUSTC:=$(STAGING_DIR)/host/bin/rustc
  CARGO:=$(STAGING_DIR)/host/bin/cargo
endif

# Directory holding libclang for bindgen: the host LLVM, the one built by
# tools/llvm-bpf or the prebuilt one unpacked into the source tree, as
# selected for the BPF toolchain.
ifdef CONFIG_USE_LLVM_HOST
  RUST_HOST_LLVM_PREFIX:=$(call qstrip,$(CONFIG_BPF_TOOLCHAIN_HOST_PATH))
  RUST_HOST_LLVM_LIBDIRS:= \
	$(if $(RUST_HOST_LLVM_PREFIX),$(RUST_HOST_LLVM_PREFIX)/lib $(RUST_HOST_LLVM_PREFIX)/lib64) \
	$(shell PATH='$(if $(RUST_HOST_LLVM_PREFIX),$(RUST_HOST_LLVM_PREFIX)/bin:)$(PATH)' \
		llvm-config --libdir 2>/dev/null) \
	/usr/lib /usr/lib64 /usr/local/lib
  RUST_LIBCLANG_PATH:=$(firstword $(foreach d,$(RUST_HOST_LLVM_LIBDIRS), \
	$(if $(wildcard $(d)/libclang.so* $(d)/libclang-*.so* $(d)/libclang.dylib),$(d))))
else ifdef CONFIG_USE_LLVM_BUILD
  RUST_LIBCLANG_PATH:=$(STAGING_DIR_HOST)/llvm-bpf/lib
else ifdef CONFIG_USE_LLVM_PREBUILT
  RUST_LIBCLANG_PATH:=$(TOPDIR)/llvm-bpf/lib
endif

# Rust Environmental Vars
RUSTC_HOST_SUFFIX:=$(word 4, $(subst -, ,$(GNU_HOST_NAME)))
RUSTC_HOST_ARCH:=$(HOST_ARCH)-unknown-linux-$(RUSTC_HOST_SUFFIX)
CARGO_HOME:=$(STAGING_DIR_HOSTPKG)/cargo

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

CARGO_RUSTFLAGS+=-Ctarget-feature=-crt-static $(RUSTC_LDFLAGS) \
	$(if $(CONFIG_DEBUG),,-Cpanic=abort -Cforce-unwind-tables=no)

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
RUST_DIST_URL:=https://static.rust-lang.org/dist/2026-10-15/
RUST_SRC_HASH:=e485a88c45db546b73571df7bebfcb4454ba4fb552c492e25fc5b00eeb5bf22d
ifeq ($(RUSTC_HOST_ARCH),x86_64-unknown-linux-gnu)
  RUST_RUSTC_HASH:=2ff05ff9c08a90861e7f6e4065b40a8b87463b4576186fd613871c3e37f2091f
  RUST_CARGO_HASH:=53f3521949404d595efb06a55c12c5658d4ed1c75566f33d3d514eaebe70bd8a
  RUST_STD_HASH:=109b074225bf5b9ea3820890876bfdf4ed161adbfaf94b2b4b5984c85ee5d958
  RUST_LLVM_TOOLS_HASH:=8429c5117debfb45c22cf2f97712ef00e0e94976c96cf92f661974a458ca7e84
else ifeq ($(RUSTC_HOST_ARCH),aarch64-unknown-linux-gnu)
  RUST_RUSTC_HASH:=acdcdfe09ae1f103aa9a2b7f88903a9588bc2e99288af10e6fbf5514e2e49f25
  RUST_CARGO_HASH:=5156832e0c1254e9b49a79edabdc6d4c35f32f8fc1cf3ca700f268ff96087ab0
  RUST_STD_HASH:=e25668477083d6a9ddb87b9beb9c00742225f38233ccb4bfba3afa7a07427f52
  RUST_LLVM_TOOLS_HASH:=cd1818058908c1c4e99bd2843fe103df255157454e16186ec9cc3b5b5823b39c
else ifeq ($(RUSTC_HOST_ARCH),x86_64-unknown-linux-musl)
  RUST_RUSTC_HASH:=034e2cfea9551483c8cedbf89e52d045b89dcf860ab73aae618c89b1a87b0f18
  RUST_CARGO_HASH:=f70f6f9b251a2d9cbb0f5792c612fde478e06cfa76abb639aceaada2c6453b1f
  RUST_STD_HASH:=1dd241eaeb19ff3b5180461c77bdee93d6cafc669eda9de834e7c06b0ec3ca95
  RUST_LLVM_TOOLS_HASH:=8881115fd0c411e3b109f814c995ad8bbe61b3f6f405629924d5d1e4c96b4912
else ifeq ($(RUSTC_HOST_ARCH),aarch64-unknown-linux-musl)
  RUST_RUSTC_HASH:=4c80eab52764172618197bd23000c739fdf561ae9415caa2a5ef62ab5c785175
  RUST_CARGO_HASH:=50f323ec9e1e60c41d42ef4dd988cc87d0a79f7b4ea10fd364df177212b89106
  RUST_STD_HASH:=2f3eb0d0610e2b7f6b2121d106e2490ac648decfa0c09b30873bdfc60de6c1ab
  RUST_LLVM_TOOLS_HASH:=aa38b41b8a9ab513269a9362f943142060c7ad626822d36a933260ed798c16da
else ifeq ($(RUSTC_HOST_ARCH),x86_64-apple-darwin)
  RUST_RUSTC_HASH:=203feef2c2165d7dda1f30ccf75590f72a6abc52ce259ed1c418de84a6887698
  RUST_CARGO_HASH:=6636dabcec164909d1c6eb9e2c596148787c84f7d3a02ff68a141569077f80b5
  RUST_STD_HASH:=484d5b88b424ddd29df196e1eb9420c8335bb5fac90a3c37140253345a8c65c4
  RUST_LLVM_TOOLS_HASH:=3112e92b03b1cca4264a30ca602948d31976cd45db7de2bc7900efa25a39a045
else ifeq ($(RUSTC_HOST_ARCH),aarch64-apple-darwin)
  RUST_RUSTC_HASH:=23948a0a9c162fd2952defb3fb0462c44e8fcfd1a4ab8a274a1e1bdb2cf810a3
  RUST_CARGO_HASH:=685e563d94db1f003ffa5f5831439eec02569b9950b69867953c8213269c1eaf
  RUST_STD_HASH:=fe79d6ab5a0321de26463999d8c112b8ad3eafbcbcf822d66a2300b45dde1031
  RUST_LLVM_TOOLS_HASH:=b348db9d79666aee4b5e2ff4269029fffd16815cbd47f47018cd0b06e4f815f5
endif
RUST_CARGO_URL:=$(RUST_DIST_URL)cargo-$(RUST_VERSION)-$(RUSTC_HOST_ARCH).tar.xz

# mips64 n64 ABI uses the muslabi64 and gnuabi64 suffixes; n32 has no Rust target;
# o32 is not standard on 64-bit
# muslabi64 is hard-float; mips64-openwrt-linux-musl is its soft-float variant.
ifeq ($(ARCH),mips64)
  RUSTC_TARGET_ARCH:=$(subst openwrt,unknown,$(REAL_GNU_TARGET_NAME))
  ifeq ($(call qstrip,$(CONFIG_MIPS64_ABI)),64)
    RUSTC_TARGET_ARCH:=$(subst musl,muslabi64,$(subst -gnu,-gnuabi64,$(RUSTC_TARGET_ARCH)))
    ifeq ($(CONFIG_USE_MUSL)$(CONFIG_SOFT_FLOAT),yy)
      RUSTC_TARGET_ARCH:=mips64-openwrt-linux-musl
    endif
  endif
else ifeq ($(ARCH),mips64el)
  RUSTC_TARGET_ARCH:=$(subst openwrt,unknown,$(REAL_GNU_TARGET_NAME))
  ifeq ($(call qstrip,$(CONFIG_MIPS64_ABI)),64)
    RUSTC_TARGET_ARCH:=$(subst musl,muslabi64,$(subst -gnu,-gnuabi64,$(RUSTC_TARGET_ARCH)))
  endif
else
  RUSTC_TARGET_ARCH:=$(subst openwrt,unknown,$(REAL_GNU_TARGET_NAME))
endif

# rust/host of the packages feed builds its mips64 std for these triples
ifndef CONFIG_USE_RUST_TOOLCHAIN
  ifeq ($(ARCH),mips64)
    RUSTC_TARGET_ARCH:=$(REAL_GNU_TARGET_NAME)
  else ifeq ($(ARCH),mips64el)
    RUSTC_TARGET_ARCH:=$(subst openwrt,unknown,$(REAL_GNU_TARGET_NAME))
  endif
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

# Cargo also links build scripts with the target linker when the target triple
# is the host's, but keeps target rustflags off them. cc-rs then compiles
# target C code with the host compiler, see scripts/rust-cc-wrapper.sh.
ifeq ($(CONFIG_USE_RUST_TOOLCHAIN)$(RUSTC_TARGET_ARCH),y$(RUSTC_HOST_ARCH))
  RUSTC_TARGET_LINKER_FLAGS:=-Clinker=$(TARGET_CC_NOCACHE)
  RUSTC_TARGET_CC_VARS:= \
	CC_$(subst -,_,$(RUSTC_TARGET_ARCH))=$(STAGING_DIR_HOST)/bin/rust-cc-wrapper \
	CXX_$(subst -,_,$(RUSTC_TARGET_ARCH))=$(STAGING_DIR_HOST)/bin/rust-cxx-wrapper \
	TARGET_CXX=$(TARGET_CXX_NOCACHE) \
	HOST_CC=$(HOSTCC_NOCACHE) \
	HOST_CXX=$(HOSTCXX_NOCACHE)
else
  RUSTC_TARGET_LINKER_VARS:= \
	CARGO_TARGET_$(subst -,_,$(call toupper,$(RUSTC_TARGET_ARCH)))_LINKER=$(TARGET_CC_NOCACHE)
endif

ifeq ($(ARCH),aarch64)
    RUSTC_CFLAGS:=-mno-outline-atomics
endif

ifneq ($(filter mips mipsel mips64 mips64el,$(ARCH)),)
  RUSTC_TARGET_CPU:=$(patsubst -%,%,$(lastword \
	$(filter -mips32% -mips64%,$(TARGET_OPTIMIZATION))))
  RUSTC_TARGET_CPU:=$(or $(RUSTC_TARGET_CPU),\
	$(patsubst -march=%,%,$(filter -march=octeon%,$(TARGET_OPTIMIZATION))),\
	$(filter mips32 mips64 mips64r2,$(call qstrip,$(CONFIG_CPU_TYPE))))
  ifneq ($(RUSTC_TARGET_CPU),)
    RUSTC_TARGET_FLAGS+=-Ctarget-cpu=$(RUSTC_TARGET_CPU)
    ifneq ($(filter mips32 mips64,$(RUSTC_TARGET_CPU)),)
      RUSTC_TARGET_FLAGS+=-Ctarget-feature=-$(RUSTC_TARGET_CPU)r2
    endif
  endif
endif

# mpc85xx (e500v2) has no FPU, but shares the hard-float powerpc triple
ifeq ($(ARCH)$(CONFIG_SOFT_FLOAT),powerpcy)
  RUSTC_TARGET_FLAGS+=-Ctarget-feature=-hard-float
endif

# Support only architectures with a valid Rust target.
# powerpc/powerpc64 with SPE FPU append 'spe' to TARGET_SUFFIX, which is an invalid target.
# mips64/mips64el n32 ABI has no standard Rust target.
# The soft-float n64 Rust target is mips64-openwrt-linux-musl only.
RUST_ARCH_DEPENDS:=@(aarch64||arm||i386||loongarch64||mips||(!MIPS64_ABI_N32&&mips64&&(USE_MUSL||!SOFT_FLOAT))||(!MIPS64_ABI_N32&&mips64el&&!SOFT_FLOAT)||mipsel||(!HAS_SPE_FPU&&powerpc)||(!HAS_SPE_FPU&&powerpc64)||riscv64||x86_64)

# LLVM codegen backends to compile into rustc, one per unique LLVM target name
# covering all architectures with a Rust target (semicolon-separated):
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
# bundling libstd.  Override per package with RUST_PREFER_DYNAMIC:=0 or 1.
# rust/host of the packages feed has no shared libstd.
RUST_LIBSTD_DYNAMIC = $(if $(CONFIG_USE_RUST_TOOLCHAIN), \
	$(or $(RUST_PREFER_DYNAMIC),$(if $(CONFIG_RUST_PREFER_DYNAMIC),1,0)),0)

RUST_IMMEDIATE_ABORT:=$(if $(CONFIG_DEBUG),,$(CONFIG_RUST_PANIC_IMMEDIATE_ABORT))

# Size options of unstable rustc, for libstd-rust and, through
# scripts/rust-target-wrapper.sh, for the crates of packages. A package keeps
# panic locations with RUST_PKG_LOCATION_DETAIL:=1, and selects the Debug
# output with RUST_PKG_FMT_DEBUG:=full or none. immediate-abort applies to all
# crates or none, std included.
RUST_UNSTABLE_FLAGS = $(if $(CONFIG_DEBUG),, \
	$(if $(filter 1,$(RUST_PKG_LOCATION_DETAIL)),,-Zlocation-detail=none) \
	-Zfmt-debug=$(or $(RUST_PKG_FMT_DEBUG),shallow) \
	$(if $(RUST_IMMEDIATE_ABORT),-Zunstable-options -Cpanic=immediate-abort))

RUST_LIBSTD_FEATURES:=$(if $(RUST_IMMEDIATE_ABORT),,panic-unwind) \
	$(if $(CONFIG_DEBUG),,optimize_for_size)
RUST_LIBSTD_FLAGS= \
	-Zforce-unstable-if-unmarked \
	-Ctarget-feature=-crt-static \
	-Ccodegen-units=1 \
	-Cembed-bitcode=yes \
	$(RUSTC_TARGET_FLAGS) \
	$(if $(CONFIG_DEBUG),,-Cpanic=abort -Cforce-unwind-tables=no -Copt-level=z -Cstrip=debuginfo \
		-Zlocation-detail=none -Zfmt-debug=shallow \
		$(if $(RUST_IMMEDIATE_ABORT),-Zunstable-options -Cpanic=immediate-abort))

RUST_LIBSTD_CONFIG:= \
	CONFIG_DEBUG CONFIG_CPU_TYPE CONFIG_TARGET_OPTIMIZATION \
	CONFIG_EXTRA_OPTIMIZATION CONFIG_GCC_VERSION CONFIG_BINUTILS_VERSION \
	CONFIG_SOFT_FLOAT CONFIG_USE_LTO CONFIG_USE_MOLD CONFIG_USE_MIPS16 \
	CONFIG_USE_GC_SECTIONS CONFIG_REPRODUCIBLE_DEBUG_INFO \
	CONFIG_RUST_PANIC_IMMEDIATE_ABORT \
	$(sort $(filter CONFIG_PKG_%,$(.VARIABLES)))

# Build paths in panic locations, file!() and debug information, mapped as
# iremap in rules.mk does for C. Local debugging keeps them, unless the debug
# information is to be reproducible.
RUST_PATH_REMAP = $(if $(and $(filter 1,$(call pkg_build_flag,iremap,1)), \
		$(if $(CONFIG_DEBUG),$(CONFIG_REPRODUCIBLE_DEBUG_INFO),y)), \
	--remap-path-prefix=$(PKG_BUILD_DIR)=$(notdir $(PKG_BUILD_DIR)) \
	--remap-path-prefix=$(STAGING_DIR)=staging_dir/$(notdir $(STAGING_DIR)))

# Identifier in the name of the shared libstd, libstd-ID.so, and ABI version
# of package/libs/libstd-rust. It changes with every input that changes the
# shared library.
LIBSTD_RUST_INPUTS = $(strip \
	$(RUST_VERSION) $(RUSTC_ID) $(RUST_SRC_TARBALL_HASH) $(RUSTC_TARGET_ARCH) \
	$(RUST_LIBSTD_FLAGS) $(RUST_LIBSTD_FEATURES) $(RUSTC_CFLAGS) \
	$(foreach v,$(RUST_LIBSTD_CONFIG),$(v)=$($(v))))
LIBSTD_RUST_ID = $(shell printf '%s' '$(subst ','\'',$(LIBSTD_RUST_INPUTS))' | $(MKHASH) sha256)
RUST_TARGET_SYSROOT = $(STAGING_DIR)/usr/lib/rust/$(LIBSTD_RUST_ID)
RUSTLIB_TARGET_PATH = /usr/lib/rust/$(LIBSTD_RUST_ID)/lib/rustlib/$(RUSTC_TARGET_ARCH)/lib
RUST_PKG_BUILD_DEPENDS:=USE_RUST_TOOLCHAIN:libstd-rust !USE_RUST_TOOLCHAIN:rust/host
RUST_HOST_BUILD_DEPENDS:=!USE_RUST_TOOLCHAIN:rust/host

RUST_PKG_CONFIG_DEPENDS:= \
	$(RUST_LIBSTD_CONFIG) \
	CONFIG_USE_RUST_TOOLCHAIN \
	$(if $(CONFIG_USE_RUST_TOOLCHAIN),CONFIG_RUST_PREFER_DYNAMIC LIBSTD_RUST_ID)

CARGO_RUSTFLAGS+= \
	$(RUSTC_TARGET_FLAGS) \
	$(RUSTC_TARGET_LINKER_FLAGS) \
	$(if $(CONFIG_USE_RUST_TOOLCHAIN),--sysroot=$(RUST_TARGET_SYSROOT)) \
	$(if $(filter 1,$(RUST_LIBSTD_DYNAMIC)),-Cprefer-dynamic) \
	$(RUST_PATH_REMAP)

# RUST_PKG_DEPENDS: use in Package/foo DEPENDS for all cargo-built target packages.
# Includes arch constraint and the libstd-rust runtime dependency when the
# package links libstd dynamically. The default is a Kconfig condition, as
# .config is not read when metadata is collected.
ifdef CONFIG_USE_RUST_TOOLCHAIN
  # rustc runs LTO next to -Cprefer-dynamic only with -Zdylib-lto.
  RUST_TARGET_WRAPPER_VARS = \
	RUSTC_WRAPPER=$(SCRIPT_DIR)/rust-target-wrapper.sh \
	RUST_TARGET_WRAPPER_FLAGS="$(RUST_UNSTABLE_FLAGS)" \
	RUST_TARGET_WRAPPER_LTO_FLAGS="$(if $(filter 1,$(RUST_LIBSTD_DYNAMIC)),-Zdylib-lto)"
endif

RUST_PKG_DEPENDS = $(RUST_ARCH_DEPENDS) \
	$(if $(RUST_PREFER_DYNAMIC), \
		$(if $(filter 1,$(RUST_PREFER_DYNAMIC)),+USE_RUST_TOOLCHAIN:libstd-rust), \
		+RUST_PREFER_DYNAMIC:libstd-rust)

CARGO_HOST_CONFIG_VARS= \
	RUSTC=$(RUSTC) \
	CARGO_HOME=$(CARGO_HOME)

CARGO_HOST_PROFILE:=release

# Packages build the release profile, as maturin, CMake and autotools projects
# do, or one inheriting from it. CONFIG_DEBUG gives it the settings of dev;
# these take precedence over the profile settings of the package.
CARGO_PKG_CONFIG_VARS= \
	RUSTC=$(RUSTC) \
	CARGO_BUILD_TARGET=$(RUSTC_TARGET_ARCH) \
	CARGO_HOME=$(CARGO_HOME) \
	$(if $(CONFIG_DEBUG),,CARGO_PROFILE_RELEASE_CODEGEN_UNITS=1) \
	CARGO_PROFILE_RELEASE_DEBUG=$(if $(CONFIG_DEBUG),true,false) \
	CARGO_PROFILE_RELEASE_DEBUG_ASSERTIONS=$(if $(CONFIG_DEBUG),true,false) \
	CARGO_PROFILE_RELEASE_LTO=$(if $(CONFIG_DEBUG),false,true) \
	$(RUST_TARGET_WRAPPER_VARS) \
	CARGO_PROFILE_RELEASE_OPT_LEVEL=$(if $(CONFIG_DEBUG),0,z) \
	CARGO_PROFILE_RELEASE_OVERFLOW_CHECKS=true \
	CARGO_PROFILE_RELEASE_PANIC=$(if $(CONFIG_DEBUG),unwind,abort) \
	CARGO_PROFILE_RELEASE_RPATH=false \
	$(RUSTC_TARGET_LINKER_VARS) \
	$(RUSTC_TARGET_CC_VARS) \
	CARGO_TARGET_$(subst -,_,$(call toupper,$(RUSTC_TARGET_ARCH)))_RUSTFLAGS="$(CARGO_RUSTFLAGS)" \
	TARGET_CC=$(TARGET_CC_NOCACHE) \
	TARGET_CFLAGS="$(TARGET_CFLAGS) $(RUSTC_CFLAGS)" \
	TARGET_CXXFLAGS="$(TARGET_CXXFLAGS) $(RUSTC_CFLAGS)"

CARGO_PKG_PROFILE:=release

# rustc drives the linker itself, without TARGET_LDFLAGS: pass the linker, and
# the -z options hardening.mk and package.mk set for the package.
RUSTC_TARGET_LINK_ARGS = \
	-Clink-arg=-fuse-ld=$(TARGET_LINKER) \
	$(addprefix -Clink-arg=,$(filter -z%,$(TARGET_LDFLAGS)))

CARGO_RUSTFLAGS+=$(RUSTC_TARGET_LINK_ARGS)

endif # __inc_rust_values

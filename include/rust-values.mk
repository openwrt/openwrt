# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2023 Luca Barbato and Donald Hoskins

ifneq ($(__inc_rust_values),1)
__inc_rust_values:=1

# Single source of truth for the Rust version
RUST_VERSION:=1.99.1
RUST_SRC_TARBALL_HASH:=6451598a99e77f07571960f6b21d577399d3a30f33e80cbd2e3a4aae6535d265

# Paths to host Rust toolchain binaries
RUSTC:=$(STAGING_DIR_HOST)/bin/rustc
CARGO:=$(STAGING_DIR_HOST)/bin/cargo

# Rust Environmental Vars
RUSTC_HOST_SUFFIX:=$(word 4, $(subst -, ,$(GNU_HOST_NAME)))
RUSTC_HOST_ARCH:=$(HOST_ARCH)-unknown-linux-$(RUSTC_HOST_SUFFIX)
CARGO_HOME:=$(STAGING_DIR_HOSTPKG)/cargo

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

CARGO_HOST_CONFIG_VARS= \
	RUSTC=$(RUSTC) \
	CARGO_HOME=$(CARGO_HOME)

CARGO_HOST_PROFILE:=release

endif # __inc_rust_values

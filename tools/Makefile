#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Main makefile for the host tools
#
curdir:=tools

# subdirectories to descend into
tools-y :=
# ...or not
tools-n :=

tools-default :=
# every default tool; should include anything that is always needed
tools-default += autoconf
tools-default += autoconf-archive
tools-default += automake
tools-default += bc
tools-default += bison
tools-default += cmake
tools-default += cpio
tools-default += dosfstools
tools-default += e2fsprogs
tools-default += elfutils
tools-default += expat
tools-default += fakeroot
tools-default += findutils
tools-default += firmware-utils
tools-default += flex
tools-default += gengetopt
tools-default += gnulib
tools-default += libressl
tools-default += libtool
tools-default += lzma
tools-default += m4
tools-default += make-ext4fs
tools-default += meson
tools-default += missing-macros
tools-default += mkimage
tools-default += mklibs
tools-default += mtd-utils
tools-default += mtools
tools-default += ninja
tools-default += padjffs2
tools-default += patchelf
tools-default += patch-image
tools-default += pkgconf
tools-default += quilt
tools-default += squashfs4
tools-default += sstrip
tools-default += util-linux
tools-default += xz
tools-default += zip
tools-default += zlib

tools-optional :=
# every optional tool; should include every dir in tools/ not in tools-default
tools-optional += 7z
tools-optional += b43-tools
tools-optional += bzip2
tools-optional += cbootimage
tools-optional += cbootimage-configs
tools-optional += elftosb
tools-optional += genext2fs
tools-optional += gmp
tools-optional += isl
tools-optional += kernel2minor
tools-optional += liblzo
tools-optional += llvm-bpf
tools-optional += lz4
tools-optional += lzma-old
tools-optional += lzop
tools-optional += mold
tools-optional += mpc
tools-optional += mpfr
tools-optional += sdimage
tools-optional += sparse
tools-optional += squashfs3-lzma

ifeq ($(CONFIG_EXTERNAL_TOOLCHAIN),)
  BUILD_TOOLCHAIN := y
  ifdef CONFIG_GCC_USE_GRAPHITE
    BUILD_ISL = y
  endif
endif
ifneq ($(CONFIG_SDK)$(CONFIG_PACKAGE_kmod-b43)$(CONFIG_BRCMSMAC_USE_FW_FROM_WL),)
  BUILD_B43_TOOLS = y
endif
ifneq ($(CONFIG_SDK)$(CONFIG_TARGET_INITRAMFS_COMPRESSION_BZIP2),)
  BUILD_BZIP2_TOOLS = y
endif
ifneq ($(CONFIG_SDK)$(CONFIG_TARGET_INITRAMFS_COMPRESSION_LZ4),)
  BUILD_LZ4_TOOLS = y
endif
ifneq ($(CONFIG_SDK)$(CONFIG_TARGET_INITRAMFS_COMPRESSION_LZO),)
  BUILD_LZO_TOOLS = y
endif

include $(INCLUDE_DIR)/list.mk

$(call List/AddUniq, tools-y, $(tools-default))

# config-dependencies:
$(call List/AddUniqIf,$(CONFIG_BUILD_ALL_HOST_TOOLS), tools, $(tools-optional))
$(call List/AddUniqIf,$(BUILD_B43_TOOLS)            , tools, b43-tools)
$(call List/AddUniqIf,$(BUILD_BZIP2_TOOLS)          , tools, bzip2)
$(call List/AddUniqIf,$(BUILD_ISL)                  , tools, isl)
$(call List/AddUniqIf,$(BUILD_LZ4_TOOLS)            , tools, lz4)
$(call List/AddUniqIf,$(BUILD_LZO_TOOLS)            , tools, lzop)
$(call List/AddUniqIf,$(BUILD_TOOLCHAIN)            , tools, gmp mpc mpfr)
$(call List/AddUniqIf,$(CONFIG_TARGET_apm821xx)     , tools, genext2fs)
$(call List/AddUniqIf,$(CONFIG_TARGET_ath79)        , tools, lzma-old squashfs3-lzma)
$(call List/AddUniqIf,$(CONFIG_TARGET_gemini)       , tools, genext2fs)
$(call List/AddUniqIf,$(CONFIG_TARGET_mxs)          , tools, elftosb sdimage)
$(call List/AddUniqIf,$(CONFIG_TARGET_realtek)      , tools, 7z)
$(call List/AddUniqIf,$(CONFIG_TARGET_tegra)        , tools, cbootimage cbootimage-configs)
$(call List/AddUniqIf,$(CONFIG_USE_LLVM_BUILD)      , tools, llvm-bpf)
$(call List/AddUniqIf,$(CONFIG_USE_MOLD)            , tools, mold)
$(call List/AddUniqIf,$(CONFIG_USE_SPARSE)          , tools, sparse)
$(call List/AddUniqIf,$(CONFIG_USES_MINOR)          , tools, kernel2minor)

# builddir dependencies
$(call List/DepAddUniq, compile, autoconf      , m4)
$(call List/DepAddUniq, compile, automake      , autoconf pkgconf xz)
$(call List/DepAddUniq, compile, b43-tools     , bison)
$(call List/DepAddUniq, compile, bc            , bison libtool)
$(call List/DepAddUniq, compile, bison         , flex)
$(call List/DepAddUniq, compile, cbootimage    , automake)
$(call List/DepAddUniq, compile, cmake         , libressl ninja expat xz zlib zstd)
$(call List/DepAddUniq, compile, dosfstools    , automake)
$(call List/DepAddUniq, compile, e2fsprogs     , libtool)
$(call List/DepAddUniq, compile, elfutils      , m4 zlib gnulib libtool)
$(call List/DepAddUniq, compile, fakeroot      , libtool)
$(call List/DepAddUniq, compile, findutils     , bison)
$(call List/DepAddUniq, compile, firmware-utils, cmake)
$(call List/DepAddUniq, compile, flex          , libtool)
$(call List/DepAddUniq, compile, genext2fs     , libtool)
$(call List/DepAddUniq, compile, gengetopt     , libtool)
$(call List/DepAddUniq, compile, gmp           , libtool)
$(call List/DepAddUniq, compile, isl           , gmp)
$(call List/DepAddUniq, compile, liblzo        , cmake)
$(call List/DepAddUniq, compile, libressl      , pkgconf)
$(call List/DepAddUniq, compile, libtool       , automake gnulib missing-macros)
$(call List/DepAddUniq, compile, llvm-bpf      , cmake)
$(call List/DepAddUniq, compile, lz4           , meson)
$(call List/DepAddUniq, compile, lzma-old      , zlib)
$(call List/DepAddUniq, compile, lzop          , cmake liblzo)
$(call List/DepAddUniq, compile, make-ext4fs   , zlib)
$(call List/DepAddUniq, compile, meson         , ninja)
$(call List/DepAddUniq, compile, missing-macros, autoconf)
$(call List/DepAddUniq, compile, mkimage       , bison libressl)
$(call List/DepAddUniq, compile, mklibs        , libtool)
$(call List/DepAddUniq, compile, mold          , cmake zlib zstd)
$(call List/DepAddUniq, compile, mpc           , mpfr gmp)
$(call List/DepAddUniq, compile, mpfr          , gmp)
$(call List/DepAddUniq, compile, mtd-utils     , libtool e2fsprogs zlib)
$(call List/DepAddUniq, compile, padjffs2      , findutils)
$(call List/DepAddUniq, compile, patchelf      , libtool)
$(call List/DepAddUniq, compile, pkgconf       , meson)
$(call List/DepAddUniq, compile, quilt         , autoconf findutils)
$(call List/DepAddUniq, compile, sdcc          , bison)
$(call List/DepAddUniq, compile, squashfs3-lzma, lzma-old)
$(call List/DepAddUniq, compile, squashfs4     , xz zlib)
$(call List/DepAddUniq, compile, util-linux    , bison)

ifneq ($(HOST_OS),Linux)
  $(call List/DepAddUniq, compile, coreutils, automake bison gnulib)
  $(call List/DepAddUniq, compile, squashfs4, coreutils)
  $(call List/AddUniq, tools-y, coreutils)
endif
ifeq ($(HOST_OS),Darwin)
  $(call List/AddUniq, tools-y, bash)
else
  $(call List/DepAddUniq, compile, dwarves , elfutils)
  $(call List/AddUniqIf, $(CONFIG_BUILD_ALL_HOST_TOOLS)$(CONFIG_DWARVES), tools, dwarves)
endif

ifneq ($(CONFIG_CCACHE)$(CONFIG_SDK),)
$(foreach tool, $(filter-out zstd zlib xz pkgconf patch ninja meson libressl expat cmake,$(tools-y)),\
  $(call List/DepAddUniq, compile, $(tool), ccache)\
)
$(call List/AddUniq, tools-y, ccache)
$(call List/DepAddUniq, compile, ccache, cmake)
endif

# in case there is no patch tool on the host we need to make patch tool a
# dependency for tools which have patches directory
$(foreach tool, $(tools-y),\
  $(if $(wildcard $(curdir)/$(tool)/patches),\
    $(call List/DepAddUniq, compile, $(tool), patch)\
  )\
)

$(foreach tool, $(filter-out zstd,$(tools-y)),\
  $(call List/DepAddUniq, compile, $(tool), zstd)\
)

# make any tool depend on the following to ensure that archives can be unpacked and patched properly
$(call List/AddUniq, tools-core, libdeflate patch tar zstd)

$(foreach tool, $(tools-y),\
  $(call List/DepAddUniq, compile, $(tool), $(tools-core))\
)
$(call List/AddUniq, tools-y, $(tools-core))

# make some core tools depend on sed and flock
$(call List/DepAddUniq, compile, patch, sed)
$(call List/DepAddUniq, compile, tar, sed)
$(call List/DepAddUniq, compile, zstd, libdeflate)

$(call List/DepAddUniq, compile, sed, flock zstd)
$(call List/AddUniq, tools-y, flock sed)

$(curdir)/autoremove := 1
$(curdir)/builddirs := $(tools-y) $(tools-dep) $(tools-n)
$(curdir)/builddirs-default := $(tools-y)

ifdef CHECK_ALL
$(curdir)/builddirs-check:=$($(curdir)/builddirs)
$(curdir)/builddirs-download:=$($(curdir)/builddirs)
endif

ifndef DUMP_TARGET_DB
define PrepareStaging
	@for dir in $(1); do ( \
		$(if $(QUIET),,set -x;) \
		mkdir -p "$$dir"; \
		cd "$$dir"; \
		mkdir -p bin lib stamp usr/include usr/lib; \
	); done
endef

$(BIN_DIR):
	mkdir -p $@

# preparatory work
$(STAGING_DIR)/.prepared: $(TMP_DIR)/.build
	$(call PrepareStaging,$(STAGING_DIR))
	mkdir -p $(BUILD_DIR)/stamp
	touch $@

$(STAGING_DIR_HOST)/.prepared: $(TMP_DIR)/.build
	$(call PrepareStaging,$(STAGING_DIR_HOST))
	mkdir -p $(BUILD_DIR_HOST)/stamp $(STAGING_DIR_HOST)/include/sys
	$(INSTALL_DATA) $(TOPDIR)/tools/include/*.h $(STAGING_DIR_HOST)/include/
	$(INSTALL_DATA) $(TOPDIR)/tools/include/sys/*.h $(STAGING_DIR_HOST)/include/sys/
ifneq ($(HOST_OS),Linux)
	mkdir -p $(STAGING_DIR_HOST)/include/asm
	$(INSTALL_DATA) $(TOPDIR)/tools/include/asm/*.h $(STAGING_DIR_HOST)/include/asm/
endif
	ln -snf lib $(STAGING_DIR_HOST)/lib64
	touch $@

endif

$(curdir)//prepare = $(STAGING_DIR)/.prepared $(STAGING_DIR_HOST)/.prepared $(BIN_DIR)
$(curdir)//compile = $(STAGING_DIR)/.prepared $(STAGING_DIR_HOST)/.prepared $(BIN_DIR)

# prerequisites for the individual targets
$(curdir)/ := .config prereq

$(curdir)/install: $(curdir)/compile

tools_enabled = $(foreach tool,$(sort $(tools-y) $(tools-n)),$(if $(filter $(tool),$(tools-y)),y,n))
$(eval $(call stampfile,$(curdir),tools,compile,,_$(subst $(space),,$(tools_enabled)),$(STAGING_DIR_HOST)))
$(eval $(call stampfile,$(curdir),tools,check,$(TMP_DIR)/.build,,$(STAGING_DIR_HOST)))
$(eval $(call subdir,$(curdir)))

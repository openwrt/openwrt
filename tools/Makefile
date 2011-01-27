#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Main makefile for the host tools
#
curdir:=tools

# subdirectories to descend into
tools-y :=
ifeq ($(CONFIG_EXTERNAL_TOOLCHAIN)$(CONFIG_GCC_LLVM),)
tools-y += gmp mpfr
tools-$(CONFIG_GCC_VERSION_4_5) += mpc libelf
endif
tools-y += m4 libtool autoconf automake flex bison pkg-config sed mklibs
tools-y += sstrip ipkg-utils genext2fs e2fsprogs mtd-utils mkimage
tools-y += firmware-utils patch-cmdline quilt yaffs2 flock
tools-y += mm-macros xorg-macros missing-macros xz
tools-$(CONFIG_TARGET_orion_generic) += wrt350nv2-builder upslug2
tools-$(CONFIG_TARGET_x86) += qemu
ifneq ($(CONFIG_TARGET_ar71xx),)
tools-y += lzma-old squashfs 
endif
tools-y += lzma squashfs4
tools-$(CONFIG_CCACHE) += ccache

ifdef CONFIG_GCC_USE_GRAPHITE
  ifeq ($(CONFIG_GCC_USE_SYSTEM_PPL_CLOOG),)
	tools-y += ppl cloog
	$(curdir)/cloog/compile := $(curdir)/ppl/install
  endif
endif

# builddir dependencies
$(curdir)/bison/compile := $(curdir)/flex/install
$(curdir)/pkg-config/compile := $(curdir)/sed/install
$(curdir)/libtool/compile := $(curdir)/sed/install
$(curdir)/squashfs/compile := $(curdir)/lzma-old/install
$(curdir)/squashfs4/compile := $(curdir)/lzma/install
$(curdir)/quilt/compile := $(curdir)/sed/install $(curdir)/autoconf/install
$(curdir)/dtc/compile := $(curdir)/bison/install
$(curdir)/autoconf/compile := $(curdir)/m4/install $(curdir)/libtool/install
$(curdir)/automake/compile := $(curdir)/m4/install $(curdir)/autoconf/install $(curdir)/pkg-config/install
$(curdir)/gmp/compile := $(curdir)/automake/install
$(curdir)/mpc/compile := $(curdir)/gmp/install
$(curdir)/mpfr/compile := $(curdir)/gmp/install
$(curdir)/ppl/compile := $(curdir)/gmp/install
$(curdir)/cloog/compile := $(curdir)/ppl/install
$(curdir)/mtd-utils/compile := $(curdir)/e2fsprogs/install
$(curdir)/mkimage/compile := $(curdir)/sed/install
$(curdir)/qemu/compile := $(curdir)/e2fsprogs/install
$(curdir)/upslug2/compile := $(curdir)/automake/install
$(curdir)/mm-macros/compile := $(curdir)/automake/install
$(curdir)/xorg-macros/compile := $(curdir)/automake/install
$(curdir)/missing-macros/compile := $(curdir)/automake/install
$(curdir)/e2fsprogs/compile := $(curdir)/automake/install
$(curdir)/libelf/compile := $(curdir)/automake/install

$(curdir)/builddirs := $(tools-y) $(tools-dep) $(tools-)
$(curdir)/builddirs-default := $(tools-y)

ifndef DUMP_TARGET_DB
define PrepareStaging
	@for dir in $(1); do ( \
		set -x; \
		mkdir -p "$$dir"; \
		cd "$$dir"; \
		mkdir -p bin lib include stamp; \
	); done
endef

# preparatory work
$(STAGING_DIR)/.prepared: $(TMP_DIR)/.build
	$(call PrepareStaging,$(STAGING_DIR))
	mkdir -p $(BUILD_DIR)/stamp
	touch $@

$(STAGING_DIR_HOST)/.prepared: $(TMP_DIR)/.build
	$(call PrepareStaging,$(STAGING_DIR_HOST))
	mkdir -p $(BUILD_DIR_HOST)/stamp
	$(INSTALL_DATA) $(TOPDIR)/tools/include/*.h $(STAGING_DIR_HOST)/include/
	touch $@


define PrepareCommand
$(STAGING_DIR_HOST)/bin/$(1): $(STAGING_DIR)/.prepared
	@mkdir -p "$$(dir $$@)"; rm -f "$$@"
	@export FILE="$$$$(which $(2) 2>/dev/null | grep -v 'not found' | head -n1)"; [ -n "$$$$FILE" ] || { \
		echo "Command $(1) not found."; false; \
	}; ln -s "$$$$FILE" "$$@"

endef
endif

$(eval $(call PrepareCommand,find,gfind find))
$(eval $(call PrepareCommand,md5sum,md5sum $(SCRIPT_DIR)/md5sum))
$(eval $(call PrepareCommand,cp,gcp cp))
$(eval $(call PrepareCommand,stat,gstat stat))

$(curdir)/cmddeps = $(patsubst %,$(STAGING_DIR_HOST)/bin/%,find md5sum cp stat)
$(curdir)//prepare = $(STAGING_DIR)/.prepared $(STAGING_DIR_HOST)/.prepared $($(curdir)/cmddeps)
$(curdir)//compile = $(STAGING_DIR)/.prepared $(STAGING_DIR_HOST)/.prepared $($(curdir)/cmddeps)

# prerequisites for the individual targets
$(curdir)/ := .config prereq
$(curdir)//install = $(1)/compile

$(eval $(call stampfile,$(curdir),tools,install,,CONFIG_CCACHE CONFIG_powerpc CONFIG_GCC_VERSION_4_5 CONFIG_GCC_USE_GRAPHITE CONFIG_TARGET_orion_generic))
$(eval $(call subdir,$(curdir)))

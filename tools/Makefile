#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Main makefile for the host tools
#
curdir:=tools

# subdirectories to descend into
tools-y :=
ifeq ($(CONFIG_EXTERNAL_TOOLCHAIN),)
tools-$(CONFIG_GCC_VERSION_4_3)$(CONFIG_GCC_VERSION_4_4) += gmp mpfr
endif
tools-y += m4 autoconf automake bison pkg-config sed mklibs
tools-y += sstrip ipkg-utils genext2fs squashfs squashfs4 mtd-utils lzma-old mkimage
tools-y += firmware-utils patch-cmdline quilt yaffs2

tools-$(CONFIG_CCACHE) += ccache
tools-dep += lzma

ifdef CONFIG_GCC_USE_GRAPHITE
  tools-y += ppl cloog
  $(curdir)/cloog/compile := $(curdir)/ppl/install
endif

# builddir dependencies
$(curdir)/pkg-config/compile := $(curdir)/sed/install
$(curdir)/squashfs/compile := $(curdir)/lzma-old/install
$(curdir)/squashfs4/compile := $(curdir)/lzma/install
$(curdir)/quilt/compile := $(curdir)/sed/install
$(curdir)/dtc/compile := $(curdir)/bison/install
$(curdir)/autoconf/compile := $(curdir)/m4/install
$(curdir)/automake/compile := $(curdir)/m4/install $(curdir)/autoconf/install
$(curdir)/mpfr/compile := $(curdir)/gmp/install

$(curdir)/builddirs := $(tools-y) $(tools-dep) $(tools-)
$(curdir)/builddirs-default := $(tools-y)

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

$(eval $(call stampfile,$(curdir),tools,install,,CONFIG_CCACHE CONFIG_powerpc CONFIG_GCC_VERSION_4_3 CONFIG_GCC_USE_GRAPHITE))
$(eval $(call subdir,$(curdir)))

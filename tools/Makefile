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
ifeq ($(CONFIG_EXTERNAL_TOOLCHAIN)$(CONFIG_GCC_LLVM),)
tools-y += gmp mpfr mpc libelf
endif
tools-y += m4 libtool autoconf automake flex bison pkg-config sed mklibs
tools-y += sstrip ipkg-utils genext2fs e2fsprogs mtd-utils mkimage
tools-y += firmware-utils patch-image quilt yaffs2 flock padjffs2
tools-y += mm-macros xorg-macros xfce-macros missing-macros xz cmake scons
tools-$(CONFIG_TARGET_orion_generic) += wrt350nv2-builder upslug2
tools-$(CONFIG_powerpc) += upx
tools-$(CONFIG_TARGET_x86) += qemu
tools-$(CONFIG_TARGET_brcm2708) += mtools dosfstools
ifneq ($(CONFIG_TARGET_ar71xx),)
tools-y += lzma-old squashfs
endif
tools-y += lzma squashfs4
ifneq ($(CONFIG_PACKAGE_kmod-b43)$(CONFIG_PACKAGE_kmod-b43legacy)$(CONFIG_BRCMSMAC_USE_FW_FROM_WL),)
tools-y += b43-tools
endif

ifneq ($(CONFIG_PACKAGE_firmwarehotplug),)
tools-y += sdcc
endif

ifdef CONFIG_GCC_USE_GRAPHITE
  ifeq ($(CONFIG_GCC_USE_SYSTEM_PPL_CLOOG),)
	tools-y += ppl cloog
	$(curdir)/cloog/compile := $(curdir)/ppl/install
  endif
endif

# builddir dependencies
$(curdir)/bison/compile := $(curdir)/flex/install
$(curdir)/flex/compile := $(curdir)/m4/install
$(curdir)/pkg-config/compile := $(curdir)/sed/install
$(curdir)/libtool/compile := $(curdir)/sed/install
$(curdir)/squashfs/compile := $(curdir)/lzma-old/install
$(curdir)/squashfs4/compile := $(curdir)/xz/install
$(curdir)/quilt/compile := $(curdir)/sed/install $(curdir)/autoconf/install
$(curdir)/dtc/compile := $(curdir)/bison/install
$(curdir)/autoconf/compile := $(curdir)/m4/install $(curdir)/libtool/install
$(curdir)/automake/compile := $(curdir)/m4/install $(curdir)/autoconf/install $(curdir)/pkg-config/install $(curdir)/xz/install
$(curdir)/gmp/compile := $(curdir)/automake/install
$(curdir)/mpc/compile := $(curdir)/mpfr/install $(curdir)/gmp/install
$(curdir)/mpfr/compile := $(curdir)/gmp/install
$(curdir)/ppl/compile := $(curdir)/gmp/install
$(curdir)/cloog/compile := $(curdir)/ppl/install
$(curdir)/mtd-utils/compile := $(curdir)/e2fsprogs/install $(curdir)/xz/install
$(curdir)/mkimage/compile := $(curdir)/sed/install
$(curdir)/qemu/compile := $(curdir)/e2fsprogs/install
$(curdir)/upslug2/compile := $(curdir)/automake/install
$(curdir)/mm-macros/compile := $(curdir)/automake/install
$(curdir)/xorg-macros/compile := $(curdir)/automake/install
$(curdir)/xfce-macros/compile := $(curdir)/automake/install
$(curdir)/missing-macros/compile := $(curdir)/automake/install
$(curdir)/e2fsprogs/compile := $(curdir)/automake/install
$(curdir)/libelf/compile := $(curdir)/automake/install
$(curdir)/sdcc/compile := $(curdir)/bison/install
$(curdir)/b43-tools/compile := $(curdir)/bison/install

ifneq ($(CONFIG_CCACHE),)
$(foreach tool, $(tools-y), $(eval $(curdir)/$(tool)/compile += $(curdir)/ccache/install))
tools-y += ccache
endif

$(curdir)/builddirs := $(tools-y) $(tools-dep) $(tools-)
$(curdir)/builddirs-default := $(tools-y)

ifndef DUMP_TARGET_DB
define PrepareStaging
	@for dir in $(1); do ( \
		$(if $(QUIET),,set -x;) \
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
	mkdir -p $(BUILD_DIR_HOST)/stamp $(STAGING_DIR_HOST)/include/sys
	$(INSTALL_DATA) $(TOPDIR)/tools/include/*.h $(STAGING_DIR_HOST)/include/
	$(INSTALL_DATA) $(TOPDIR)/tools/include/sys/*.h $(STAGING_DIR_HOST)/include/sys/
	ln -sf lib $(STAGING_DIR_HOST)/lib64
	touch $@


define PrepareCommand
$(STAGING_DIR_HOST)/bin/$(1): $(STAGING_DIR)/.prepared
	@mkdir -p "$$(dir $$@)"; rm -f "$$@"
	@export FILE="$$$$(which $(2) 2>/dev/null | grep -v 'not found' | head -n1)"; [ -n "$$$$FILE" ] || { \
		echo "Command $(1) not found."; false; \
	}; ln -s "$$$$FILE" "$$@"

endef
endif

$(STAGING_DIR_HOST)/bin/stat: $(STAGING_DIR)/.prepared
	@rm -f $@
	@if stat --version > /dev/null 2>&1; then \
		ln -s `which stat` $@; \
	elif gstat --version > /dev/null 2>&1; then \
		ln -s `which gstat` $@; \
	else \
		echo "GNU stat not found"; \
		false; \
	fi

$(eval $(call PrepareCommand,find,gfind find))
$(eval $(call PrepareCommand,md5sum,gmd5sum md5sum $(SCRIPT_DIR)/md5sum))
$(eval $(call PrepareCommand,cp,gcp cp))
$(eval $(call PrepareCommand,seq,gseq seq))
$(eval $(call PrepareCommand,python,python2 python))
$(eval $(call PrepareCommand,awk,gawk awk))

$(curdir)/cmddeps = $(patsubst %,$(STAGING_DIR_HOST)/bin/%,find md5sum cp stat seq python awk)
$(curdir)//prepare = $(STAGING_DIR)/.prepared $(STAGING_DIR_HOST)/.prepared $($(curdir)/cmddeps)
$(curdir)//compile = $(STAGING_DIR)/.prepared $(STAGING_DIR_HOST)/.prepared $($(curdir)/cmddeps)

# prerequisites for the individual targets
$(curdir)/ := .config prereq
$(curdir)//install = $(1)/compile

$(eval $(call stampfile,$(curdir),tools,install,,CONFIG_CCACHE CONFIG_powerpc CONFIG_GCC_VERSION_4_5 CONFIG_GCC_USE_GRAPHITE CONFIG_TARGET_orion_generic))
$(eval $(call subdir,$(curdir)))

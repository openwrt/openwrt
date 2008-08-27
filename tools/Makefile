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
$(curdir)/builddirs := sed sstrip ipkg-utils genext2fs squashfs mtd-utils lzma mkimage firmware-utils patch-cmdline pkg-config automake $(if $(CONFIG_CCACHE),ccache) bison $(if $(CONFIG_powerpc),dtc) lua quilt autoconf

# builddir dependencies
$(curdir)/squashfs/compile := $(curdir)/lzma/install

# preparatory work
$(STAGING_DIR)/.prepared: $(TMP_DIR)/.build
	@for dir in $(STAGING_DIR) $(STAGING_DIR_HOST); do ( \
		set -x; \
		mkdir -p "$$dir"; \
		cd "$$dir"; \
		mkdir -p bin lib include stamp; \
	); done
	mkdir -p $(BUILD_DIR_HOST)/stamp $(BUILD_DIR)/stamp
	$(INSTALL_DATA) $(TOPDIR)/tools/include/*.h $(STAGING_DIR_HOST)/include/
	touch $@

$(curdir)//prepare = $(STAGING_DIR)/.prepared
$(curdir)//compile = $(STAGING_DIR)/.prepared

# prerequisites for the individual targets
$(curdir)/ := .config prereq
$(curdir)//install = $(1)/compile

$(eval $(call stampfile,$(curdir),tools,install))
$(eval $(call subdir,$(curdir)))

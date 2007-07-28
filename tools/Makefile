# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Main makefile for the host tools
#

curdir:=$(patsubst %/Makefile,%,$(lastword $(MAKEFILE_LIST)))

# subdirectories to descend into
$(curdir)/builddirs := sed sstrip ipkg-utils ext2fs squashfs mtd-utils lzma mkimage firmware-utils patch-cmdline $(if $(CONFIG_CCACHE),ccache)

# builddir dependencies
$(curdir)/squashfs/compile := $(curdir)/lzma/install

# preparatory work
$(STAGING_DIR)/include-host/.done:
	@mkdir -p $$(dirname $@)
	@cp $(curdir)/include/*.h $$(dirname $@)/
	@touch $@

# prerequisites for the individual targets
$(curdir)/ := .config
$(curdir)//prepare = $(STAGING_DIR)/include-host/.done
$(curdir)//compile = $(1)/prepare
$(curdir)//install = $(1)/compile

$(eval $(call stampfile,$(curdir),tools))
$(eval $(call subdir,$(curdir)))

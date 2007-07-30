# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Main makefile for the host tools
#

file:=${lastword ${MAKEFILE_LIST}}
curdir:=tools

# subdirectories to descend into
$(curdir)/builddirs := sed sstrip ipkg-utils ext2fs squashfs mtd-utils lzma mkimage firmware-utils patch-cmdline $(if $(CONFIG_CCACHE),ccache)

# builddir dependencies
$(curdir)/squashfs/compile := $(curdir)/lzma/install

# preparatory work
define copy_include
$(STAGING_DIR)/include-host/.done:
	@mkdir -p $$$$(dirname $$@)
	@cp $(1)/include/*.h $$$$(dirname $$@)/
	@touch $$@
$(curdir)//prepare = $(STAGING_DIR)/include-host/.done
endef
$(eval $(call copy_include,$(curdir)))

# prerequisites for the individual targets
$(curdir)/ := .config
$(curdir)//compile = $(1)/prepare
$(curdir)//install = $(1)/compile

$(eval $(call stampfile,$(curdir),tools))
$(eval $(call subdir,$(curdir)))

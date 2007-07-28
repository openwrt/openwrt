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
tools/builddirs := sed sstrip ipkg-utils ext2fs squashfs mtd-utils lzma mkimage firmware-utils patch-cmdline $(if $(CONFIG_CCACHE),ccache)

# builddir dependencies
tools/squashfs/compile := tools/lzma/install

# preparatory work
$(STAGING_DIR)/include-host/.done:
	@mkdir -p $$(dirname $@)
	@cp tools/include/*.h $$(dirname $@)/
	@touch $@

# prerequisites for the individual targets
tools/ := .config
tools//prepare = $(STAGING_DIR)/include-host/.done
tools//compile = $(1)/prepare
tools//install = $(1)/compile

$(eval $(call stampfile,tools,tools))
$(eval $(call subdir,tools))

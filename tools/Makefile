# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Main makefile for the host tools
#
include $(TOPDIR)/rules.mk
TARGETS-y:=sed sstrip ipkg-utils ext2fs squashfs mtd-utils lzma mkimage firmware-utils
TARGETS-$(CONFIG_CCACHE) += ccache

TARGETS_DOWNLOAD:=$(patsubst %,%-download,$(TARGETS-y))
TARGETS_PREPARE:=$(patsubst %,%-prepare,$(TARGETS-y))
TARGETS_COMPILE:=$(patsubst %,%-compile,$(TARGETS-y))
TARGETS_INSTALL:=$(patsubst %,%-install,$(TARGETS-y))
TARGETS_CLEAN:=$(patsubst %,%-clean,$(TARGETS-y))
STAMP:=$(STAGING_DIR)/stampfiles/.tools_installed

all: install
download: $(TARGETS_DOWNLOAD)
compile: $(TARGETS_COMPILE)
install: $(STAMP)
clean: $(TARGETS_CLEAN)

squashfs-compile: lzma-install

ifneq ($(shell $(SCRIPT_DIR)/timestamp.pl -p . $(STAMP)),$(STAMP))
  $(STAMP): $(TARGETS_INSTALL)
endif

$(STAMP): $(TOPDIR)/.config
	mkdir -p $(shell dirname $@)
	touch $@
	
$(STAGING_DIR)/include-host:
	@mkdir -p $@
	@$(CP) ./include/*.h $@/

$(TOOL_BUILD_DIR):
	@mkdir -p $@

$(eval $(call default_subtargets,$(STAGING_DIR)/include-host $(TOOL_BUILD_DIR)))

ifeq ($(MAKECMDGOALS),install-targets)
MAKEFLAGS:=$(MAKEFLAGS) -j$(CONFIG_JLEVEL)
else
.NOTPARALLEL:
endif

# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Main makefile for the host tools
#
include $(TOPDIR)/rules.mk
TARGETS-y:=sed sstrip ipkg-utils ext2fs squashfs mtd-utils lzma mkimage

TARGETS_DOWNLOAD:=$(patsubst %,%-download,$(TARGETS-y))
TARGETS_PREPARE:=$(patsubst %,%-prepare,$(TARGETS-y))
TARGETS_INSTALL:=$(patsubst %,%-install,$(TARGETS-y))
TARGETS_CLEAN:=$(patsubst %,%-clean,$(TARGETS-y))

all: install
download: $(TARGETS_DOWNLOAD)
install: 
	$(MAKE) install-targets
install-targets: $(TARGETS_INSTALL)
clean: $(TARGETS_CLEAN)

squashfs-compile: lzma-install

TOOL_STAMP_DIR:=$(STAGING_DIR)/stampfiles

$(TOOL_STAMP_DIR):
	mkdir -p $@

$(STAGING_DIR)/include-host:
	@mkdir -p $@
	@$(CP) ./include/*.h $@/

$(TOOL_BUILD_DIR):
	@mkdir -p $@

%-download: FORCE
	$(MAKE) -C $(patsubst %-download,%,$@) download

%-prepare: $(TOOL_STAMP_DIR) $(STAGING_DIR)/include-host $(TOOL_BUILD_DIR) FORCE
	@[ -f $(TOOL_STAMP_DIR)/.tool_$@ ] || { \
		$(MAKE) -C $(patsubst %-prepare,%,$@) prepare; \
	}
	@touch $(TOOL_STAMP_DIR)/.tool_$@

%-compile: %-prepare 
	@[ -f $(TOOL_STAMP_DIR)/.tool_$@ ] || { \
		$(MAKE) -C $(patsubst %-compile,%,$@) compile; \
	}
	@touch $(TOOL_STAMP_DIR)/.tool_$@

%-install: %-compile
	@[ -f $(TOOL_STAMP_DIR)/.tool_$@ ] || { \
		$(MAKE) -C $(patsubst %-install,%,$@) install; \
	}
	@touch $(TOOL_STAMP_DIR)/.tool_$@

%-clean: FORCE
	@$(MAKE) -C $(patsubst %-clean,%,$@) clean
	@rm -f $(TOOL_STAMP_DIR)/.tool_$(patsubst %-clean,%,$@)-*

ifeq ($(MAKECMDGOALS),install-targets)
MAKEFLAGS:=$(MAKEFLAGS) -j$(CONFIG_JLEVEL)
else
.NOTPARALLEL:
endif

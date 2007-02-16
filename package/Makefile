# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(TOPDIR)/.config
include $(TMP_DIR)/.pkgdeps
include $(INCLUDE_DIR)/host.mk

PREREQ_PACKAGES:=$(patsubst %,%-prereq,$(prereq-y) $(prereq-m))
DOWNLOAD_PACKAGES:=$(patsubst %,%-download,$(package-y) $(package-m))
COMPILE_PACKAGES:=$(patsubst %,%-compile,$(package-y) $(package-m))
INSTALL_PACKAGES:=$(patsubst %,%-install,$(package-y))

$(STAMP_DIR) $(TARGET_DIR):
	mkdir -p $@

%-prereq: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-prereq,%,$@) prereq

%-download: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-download,%,$@) download

%-prepare: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-prepare,%,$@) prepare

%-compile: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-compile,%,$@) compile

%-install: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-install,%,$@) install

%-clean: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-clean,%,$@) clean

ifeq ($(SDK),1)
GENDEP_OPTS := -s
endif

$(TMP_DIR)/.pkgdeps: $(TMP_DIR)/.pkginfo
	@$(TOPDIR)/scripts/gen_deps.pl $(GENDEP_OPTS) < $< > $@ || rm -f $@

all: compile
clean: $(patsubst %,%-clean,$(package-) $(package-y) $(package-m))
prereq: $(PREREQ_PACKAGES)
download: $(DOWNLOAD_PACKAGES)
compile: $(COMPILE_PACKAGES)

install-targets: $(INSTALL_PACKAGES)
install:
	rm -rf $(BUILD_DIR)/root
	$(MAKE) install-targets
	@if [ -d $(TOPDIR)/files ]; then \
		$(CP) $(TOPDIR)/files/. $(BUILD_DIR)/root; \
	fi
	@( \
		cd $(BUILD_DIR)/root; \
		for script in ./etc/init.d/*; do \
			grep '#!/bin/sh /etc/rc.common' $$script >/dev/null || continue; \
			IPKG_INSTROOT=$(BUILD_DIR)/root $(which bash) ./etc/rc.common $$script enable; \
		done; \
	)

index: $(PACKAGE_DIR)/Packages

$(PACKAGE_DIR)/Packages: $(PACKAGE_DIR)/*.ipk
	(cd $(PACKAGE_DIR); $(SCRIPT_DIR)/ipkg-make-index.sh . > Packages)

symlinks:
	../scripts/feeds.sh $(CONFIG_SOURCE_FEEDS) 

ifeq ($(MAKECMDGOALS),compile-targets)
MAKEFLAGS:=$(MAKEFLAGS) -j$(CONFIG_JLEVEL)
else
.NOTPARALLEL:
endif

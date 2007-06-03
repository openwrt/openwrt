# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(TOPDIR)/.config
include $(INCLUDE_DIR)/host.mk

all: compile

include $(TMP_DIR)/.packagedeps

PREREQ_PACKAGES:=$(patsubst %,%-prereq,$(prereq-y) $(prereq-m))
DOWNLOAD_PACKAGES:=$(patsubst %,%-download,$(package-y) $(package-m))
COMPILE_PACKAGES:=$(patsubst %,%-compile,$(package-y) $(package-m))
INSTALL_PACKAGES:=$(patsubst %,%-install,$(package-y))

$(STAMP_DIR) $(TARGET_DIR):
	mkdir -p $@

ifeq ($(QUIET),1)
%-compile %-install: FORCE
	$(MAKE) -C $* $(patsubst $*-%,%,$@) || { $(call MESSAGE, "*** $* $(patsubst $*-%,%,$@) failed"); false; }

%-prepare %-prereq %-download %-clean %-refresh: FORCE
else
%-prepare %-prereq %-download %-clean %-compile %-install %-refresh: FORCE
endif
	$(MAKE) -C $* $(patsubst $*-%,%,$@)

# .IGNORE: $(COMPILE_PACKAGES) 

$(TMP_DIR)/.packagedeps: $(TMP_DIR)/.packageinfo
	@$(TOPDIR)/scripts/metadata.pl package_mk < $< > $@ || rm -f $@

preconfig:
clean: $(patsubst %,%-clean,$(package-) $(package-y) $(package-m))
prereq: $(PREREQ_PACKAGES)
download: $(DOWNLOAD_PACKAGES)
compile: $(COMPILE_PACKAGES)

install-targets: $(INSTALL_PACKAGES)
install:
	rm -rf $(BUILD_DIR)/root
	$(MAKE) install-targets
	$(MAKE) preconfig
	@if [ -d $(TOPDIR)/files ]; then \
		$(CP) $(TOPDIR)/files/. $(BUILD_DIR)/root; \
	fi
	@mkdir -p $(BUILD_DIR)/root/etc/rc.d
	@( \
		cd $(BUILD_DIR)/root; \
		for script in ./etc/init.d/*; do \
			grep '#!/bin/sh /etc/rc.common' $$script >/dev/null || continue; \
			IPKG_INSTROOT=$(BUILD_DIR)/root $(which bash) ./etc/rc.common $$script enable; \
		done || true \
	)
	@-find $(BUILD_DIR)/root -name CVS   | $(XARGS) rm -rf
	@-find $(BUILD_DIR)/root -name .svn  | $(XARGS) rm -rf
	@-find $(BUILD_DIR)/root -name '.#*' | $(XARGS) rm -f

index: $(PACKAGE_DIR)/Packages

$(PACKAGE_DIR)/Packages: $(PACKAGE_DIR)/*.ipk
	(cd $(PACKAGE_DIR); $(SCRIPT_DIR)/ipkg-make-index.sh . > Packages)

symlinks:
	../scripts/feeds.sh $(CONFIG_SOURCE_FEEDS) 

ifeq ($(MAKECMDGOALS),compile)
MAKEFLAGS:=$(MAKEFLAGS) -j$(CONFIG_JLEVEL)
else
.NOTPARALLEL:
endif

# 
# Copyright (C) 2006-2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

curdir:=package

-include $(TMP_DIR)/.packagedeps
$(curdir)/builddirs:=$(sort $(package-) $(package-y) $(package-m))
$(curdir)/builddirs-default:=. $(sort $(package-y) $(package-m))
$(curdir)/builddirs-prereq:=. $(sort $(prereq-y) $(prereq-m))
$(curdir)/builddirs-install:=. $(sort $(package-y))

$(curdir)//compile = $(1)/prepare
$(curdir)//install = $(1)/compile
$(curdir)/install:=$(curdir)/install-cleanup
$(curdir)/install-cleanup:
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

$(curdir)/index: FORCE
	(cd $(PACKAGE_DIR); $(SCRIPT_DIR)/ipkg-make-index.sh . > Packages)

$(curdir)/flags-install:= -j1

$(eval $(call stampfile,$(curdir),package,prereq))
$(eval $(call stampfile,$(curdir),package,compile))
$(eval $(call stampfile,$(curdir),package,install))

$($(curdir)/stamp-install): $($(curdir)/stamp-compile)

$(eval $(call subdir,$(curdir)))

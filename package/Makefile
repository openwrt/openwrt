#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

curdir:=package

-include $(TMP_DIR)/.packagedeps
$(curdir)/builddirs:=$(sort $(package-) $(package-y) $(package-m))
ifeq ($(SDK),1)
  $(curdir)/builddirs-install:=.
else
  $(curdir)/builddirs-default:=. $(sort $(package-y) $(package-m))
  $(curdir)/builddirs-prereq:=. $(sort $(prereq-y) $(prereq-m))
  $(curdir)/builddirs-install:=. $(sort $(package-y))
endif
ifneq ($(IGNORE_ERRORS),)
  $(curdir)/builddirs-ignore-compile:= $(if $(filter m y, $(IGNORE_ERRORS)),$(foreach m,$(IGNORE_ERRORS),$(package-$(m))),$(package-m))
endif

$(curdir)/install:=$(curdir)/install-cleanup

$(curdir)/cleanup: $(TMP_DIR)/.build
	rm -rf $(TARGET_DIR)

$(curdir)/rootfs-prepare: $(TMP_DIR)/.build
	@-$(MAKE) package/preconfig
	@if [ -d $(TOPDIR)/files ]; then \
		$(CP) $(TOPDIR)/files/. $(TARGET_DIR); \
	fi
	@mkdir -p $(TARGET_DIR)/etc/rc.d
	@( \
		cd $(TARGET_DIR); \
		for script in ./etc/init.d/*; do \
			grep '#!/bin/sh /etc/rc.common' $$script >/dev/null || continue; \
			IPKG_INSTROOT=$(TARGET_DIR) $$(which bash) ./etc/rc.common $$script enable; \
		done || true \
	)
	@-find $(TARGET_DIR) -name CVS   | $(XARGS) rm -rf
	@-find $(TARGET_DIR) -name .svn  | $(XARGS) rm -rf
	@-find $(TARGET_DIR) -name '.#*' | $(XARGS) rm -f
	$(if $(CONFIG_CLEAN_IPKG),rm -rf $(TARGET_DIR)/usr/lib/ipkg)

$(curdir)/index: FORCE
	@(cd $(PACKAGE_DIR); $(SCRIPT_DIR)/ipkg-make-index.sh . > Packages && \
		gzip -9c Packages > Packages.gz \
	)

$(curdir)/flags-install:= -j1

$(eval $(call stampfile,$(curdir),package,prereq,.config))
$(eval $(call stampfile,$(curdir),package,cleanup,$(TMP_DIR)/.build))
$(eval $(call stampfile,$(curdir),package,compile,$(TMP_DIR)/.build))
$(eval $(call stampfile,$(curdir),package,install,$(TMP_DIR)/.build))
$(eval $(call stampfile,$(curdir),package,rootfs-prepare,$(TMP_DIR)/.build))

$(eval $(call subdir,$(curdir)))

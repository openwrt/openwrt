#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

curdir:=package

include $(INCLUDE_DIR)/feeds.mk

-include $(TMP_DIR)/.packagedeps
$(curdir)/builddirs:=$(sort $(package-) $(package-y) $(package-m))
$(curdir)/builddirs-install:=.
$(curdir)/builddirs-default:=. $(sort $(package-y) $(package-m))
$(curdir)/builddirs-prereq:=. $(sort $(prereq-y) $(prereq-m))
ifneq ($(IGNORE_ERRORS),)
  package-y-filter := $(package-y)
  package-m-filter := $(filter-out $(package-y),$(package-m))
  package-n-filter := $(filter-out $(package-y) $(package-m),$(package-))
  package-ignore-errors := $(filter n m y,$(IGNORE_ERRORS))
  package-ignore-errors := $(if $(package-ignore-errors),$(package-ignore-errors),n m)
  $(curdir)/builddirs-ignore-download := $(foreach m,$(package-ignore-errors),$(package-$(m)-filter))
  $(curdir)/builddirs-ignore-compile := $(foreach m,$(package-ignore-errors),$(package-$(m)-filter))
endif

ifdef CONFIG_USE_MKLIBS
  define mklibs
	rm -rf $(TMP_DIR)/mklibs-progs $(TMP_DIR)/mklibs-out
	# first find all programs and add them to the mklibs list
	find $(STAGING_DIR_ROOT) -type f -perm /100 -exec \
		file -r -N -F '' {} + | \
		awk ' /executable.*dynamically/ { print $$1 }' > $(TMP_DIR)/mklibs-progs
	# find all loadable objects that are not regular libraries and add them to the list as well
	find $(STAGING_DIR_ROOT) -type f -name \*.so\* -exec \
		file -r -N -F '' {} + | \
		awk ' /shared object/ { print $$1 }' > $(TMP_DIR)/mklibs-libs
	mkdir -p $(TMP_DIR)/mklibs-out
	$(STAGING_DIR_HOST)/bin/mklibs -D \
		-d $(TMP_DIR)/mklibs-out \
		--sysroot $(STAGING_DIR_ROOT) \
		`cat $(TMP_DIR)/mklibs-libs | sed 's:/*[^/]\+/*$$::' | uniq | sed 's:^$(STAGING_DIR_ROOT):-L :'` \
		--ldlib $(patsubst $(STAGING_DIR_ROOT)/%,/%,$(firstword $(wildcard \
			$(foreach name,ld-uClibc.so.* ld-linux.so.* ld-*.so, \
			  $(STAGING_DIR_ROOT)/lib/$(name) \
			)))) \
		--target $(REAL_GNU_TARGET_NAME) \
		`cat $(TMP_DIR)/mklibs-progs $(TMP_DIR)/mklibs-libs` 2>&1
	$(RSTRIP) $(TMP_DIR)/mklibs-out
	for lib in `ls $(TMP_DIR)/mklibs-out/*.so.* 2>/dev/null`; do \
		LIB="$${lib##*/}"; \
		DEST="`ls "$(TARGET_DIR)/lib/$$LIB" "$(TARGET_DIR)/usr/lib/$$LIB" 2>/dev/null`"; \
		[ -n "$$DEST" ] || continue; \
		echo "Copying stripped library $$lib to $$DEST"; \
		cp "$$lib" "$$DEST" || exit 1; \
	done
  endef
endif

# where to build (and put) .ipk packages
OPKG:= \
  IPKG_TMP=$(TMP_DIR)/ipkg \
  IPKG_INSTROOT=$(TARGET_DIR) \
  IPKG_CONF_DIR=$(STAGING_DIR)/etc \
  IPKG_OFFLINE_ROOT=$(TARGET_DIR) \
  $(XARGS) $(STAGING_DIR_HOST)/bin/opkg \
	--offline-root $(TARGET_DIR) \
	--force-depends \
	--force-overwrite \
	--force-postinstall \
	--force-maintainer \
	--add-dest root:/ \
	--add-arch all:100 \
	--add-arch $(if $(ARCH_PACKAGES),$(ARCH_PACKAGES),$(BOARD)):200

PACKAGE_INSTALL_FILES:= \
	$(foreach pkg,$(sort $(package-y)), \
		$(foreach variant, \
			$(if $(strip $(package/$(pkg)/variants)), \
				$(package/$(pkg)/variants), \
				$(if $(package/$(pkg)/default-variant), \
					$(package/$(pkg)/default-variant), \
					default \
				) \
			), \
			$(PKG_INFO_DIR)/$(lastword $(subst /,$(space),$(pkg))).$(variant).install \
		) \
	)

$(curdir)/cleanup: $(TMP_DIR)/.build
	rm -rf $(STAGING_DIR_ROOT)

$(curdir)/install: $(TMP_DIR)/.build
	- find $(STAGING_DIR_ROOT) -type d | $(XARGS) chmod 0755
	rm -rf $(TARGET_DIR)
	[ -d $(TARGET_DIR)/tmp ] || mkdir -p $(TARGET_DIR)/tmp
	@$(FIND) `sed -e 's|.*|$(PACKAGE_DIR)$(if $(CONFIG_PER_FEED_REPO),/*)/&_*.ipk|' $(PACKAGE_INSTALL_FILES)` | sort -u | $(OPKG) install
	@for file in $(PACKAGE_INSTALL_FILES); do \
		[ -s $$file.flags ] || continue; \
		for flag in `cat $$file.flags`; do \
			$(OPKG) flag $$flag < $$file; \
		done; \
	done || true
	@-$(MAKE) package/preconfig
	@if [ -d $(TOPDIR)/files ]; then \
		$(call file_copy,$(TOPDIR)/files/.,$(TARGET_DIR)); \
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
	rm -f $(TARGET_DIR)/usr/lib/opkg/info/*.postinst*
	rm -f $(TARGET_DIR)/usr/lib/opkg/info/*.prerm*
	$(if $(CONFIG_CLEAN_IPKG),rm -rf $(TARGET_DIR)/usr/lib/opkg)
	$(call mklibs)

PASSOPT=""
PASSARG=""
ifndef CONFIG_OPKGSMIME_PASSPHRASE
  ifneq ($(call qstrip,$(CONFIG_OPKGSMIME_PASSFILE)),)
    PASSOPT="-passin"
    PASSARG="file:$(call qstrip,$(CONFIG_OPKGSMIME_PASSFILE))"
  endif
endif

PACKAGE_SUBDIRS=.
ifneq ($(CONFIG_PER_FEED_REPO),)
  PACKAGE_SUBDIRS=base $(FEEDS_ENABLED)
endif

$(curdir)/index: FORCE
	@echo Generating package index...
	@for d in $(PACKAGE_SUBDIRS); do ( \
		[ -d $(PACKAGE_DIR)/$$d ] && \
			cd $(PACKAGE_DIR)/$$d || continue; \
		$(SCRIPT_DIR)/ipkg-make-index.sh . 2>&1 > Packages && \
			gzip -9c Packages > Packages.gz; \
	); done
ifeq ($(call qstrip,$(CONFIG_OPKGSMIME_KEY)),)
	@echo Signing key has not been configured
else
ifeq ($(call qstrip,$(CONFIG_OPKGSMIME_CERT)),)
	@echo Certificate has not been configured
else
	@echo Signing package index...
	@for d in $(PACKAGE_SUBDIRS); do ( \
		[ -d $(PACKAGE_DIR)/$$d ] && \
			cd $(PACKAGE_DIR)/$$d || continue; \
		openssl smime -binary -in Packages.gz \
			-out Packages.sig -outform PEM -sign \
			-signer $(CONFIG_OPKGSMIME_CERT) \
			-inkey $(CONFIG_OPKGSMIME_KEY) \
			$(PASSOPT) $(PASSARG); \
	); done
endif
endif

$(curdir)/preconfig:

$(curdir)/flags-install:= -j1

$(eval $(call stampfile,$(curdir),package,prereq,.config))
$(eval $(call stampfile,$(curdir),package,cleanup,$(TMP_DIR)/.build))
$(eval $(call stampfile,$(curdir),package,compile,$(TMP_DIR)/.build))
$(eval $(call stampfile,$(curdir),package,install,$(TMP_DIR)/.build))

$(eval $(call subdir,$(curdir)))

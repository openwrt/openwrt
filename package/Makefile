#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

curdir:=package

include $(INCLUDE_DIR)/feeds.mk
include $(INCLUDE_DIR)/rootfs.mk

-include $(TMP_DIR)/.packagedeps
package-y += kernel/linux
$(curdir)/autoremove:=1
$(curdir)/builddirs:=$(sort $(package-) $(package-y) $(package-m))
$(curdir)/builddirs-default:=. $(sort $(package-y) $(package-m))
$(curdir)/builddirs-prereq:=. $(sort $(prereq-y) $(prereq-m))
ifdef CHECK_ALL
$(curdir)/builddirs-check:=$($(curdir)/builddirs)
$(curdir)/builddirs-download:=$($(curdir)/builddirs)
endif
ifneq ($(IGNORE_ERRORS),)
  package-y-filter := $(package-y)
  package-m-filter := $(filter-out $(package-y),$(package-m))
  package-n-filter := $(filter-out $(package-y) $(package-m),$(package-))
  package-ignore-errors := $(filter n m y,$(IGNORE_ERRORS))
  package-ignore-errors := $(if $(package-ignore-errors),$(package-ignore-errors),n m)
  package-ignore-subdirs := $(sort $(foreach m,$(package-ignore-errors),$(package-$(m)-filter)))
  $(curdir)/builddirs-ignore-download := $(package-ignore-subdirs)
  $(curdir)/builddirs-ignore-compile := $(package-ignore-subdirs)
  $(curdir)/builddirs-ignore-host-download := $(package-ignore-subdirs)
  $(curdir)/builddirs-ignore-host-compile := $(package-ignore-subdirs)
endif

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

PACKAGE_EXT:=$(if $(CONFIG_USE_APK),apk,ipk)
PACKAGE_INDEX_FILE:=$(PACKAGE_DIR_ALL)/$(if $(CONFIG_USE_APK),packages.adb,Packages)
PACKAGE_MERGE_LIST:=$(PACKAGE_DIR_ALL).list

define package_merge_links
	rm -rf $(PACKAGE_DIR_ALL); \
	mkdir -p $(PACKAGE_DIR_ALL); \
	$(foreach pdir,$(PACKAGE_SUBDIRS), \
		$(if $(wildcard $(pdir)/*.$(PACKAGE_EXT)), \
			ln -s $(pdir)/*.$(PACKAGE_EXT) $(PACKAGE_DIR_ALL) || true;))
endef

$(curdir)/merge:
	mkdir -p $(dir $(PACKAGE_MERGE_LIST))
	find $(PACKAGE_SUBDIRS) -maxdepth 1 -name '*.$(PACKAGE_EXT)' \
		-printf '%p %s %T@\n' 2>/dev/null | LC_ALL=C sort > $(PACKAGE_MERGE_LIST).new
	if cmp -s $(PACKAGE_MERGE_LIST).new $(PACKAGE_MERGE_LIST) && \
	   [ -f $(PACKAGE_INDEX_FILE) ]; then \
		rm -f $(PACKAGE_MERGE_LIST).new; \
	else \
		$(package_merge_links) \
		mv $(PACKAGE_MERGE_LIST).new $(PACKAGE_MERGE_LIST); \
	fi

$(BUILD_KEY_APK_SEC):
	$(STAGING_DIR_HOST)/bin/openssl ecparam -name prime256v1 -genkey -noout -out $(BUILD_KEY_APK_SEC)

$(BUILD_KEY_APK_PUB): $(BUILD_KEY_APK_SEC)
	$(STAGING_DIR_HOST)/bin/openssl ec -in $(BUILD_KEY_APK_SEC) -pubout > $(BUILD_KEY_APK_PUB)

$(curdir)/merge-index: $(curdir)/merge
ifneq ($(CONFIG_USE_APK),)
	[ -f $(PACKAGE_INDEX_FILE) ] || \
	(cd $(PACKAGE_DIR_ALL) && $(STAGING_DIR_HOST)/bin/apk mkndx \
			--root $(TOPDIR) \
			--keys-dir $(TOPDIR) \
			--allow-untrusted \
			$(if $(CONFIG_SIGNED_PACKAGES),--sign $(BUILD_KEY_APK_SEC),) \
			--output packages.adb \
			*.apk; \
	)
else
	[ -f $(PACKAGE_INDEX_FILE) ] || \
	(cd $(PACKAGE_DIR_ALL) && $(SCRIPT_DIR)/ipkg-make-index.sh . 2>&1 > Packages; )
endif

ifndef SDK
  $(curdir)//compile = $(STAGING_DIR)/.prepared $(BIN_DIR)
ifneq ($(CONFIG_USE_APK),)
  $(curdir)//compile += $(curdir)/system/apk/host/compile $(BUILD_KEY_APK_SEC) $(BUILD_KEY_APK_PUB)
else
  $(curdir)/compile: $(curdir)/system/opkg/host/compile
endif
else
ifneq ($(CONFIG_USE_APK),)
  $(curdir)//compile += $(BUILD_KEY_APK_SEC) $(BUILD_KEY_APK_PUB)
endif
endif

# The rootfs is a function of the package index, which holds a hash of every
# package, and of the inputs below. Installing it takes a few seconds, so
# keep a copy and restore it if none of them changed. Later build steps
# modify the rootfs, so it is always restored from the copy.
ROOTFS_CACHE:=$(TARGET_ROOTFS_DIR)/rootfs-cache-$(BOARD)

define rootfs_cache_key
$(shell ( \
	echo '$(subst ','\'',$(1))'; \
	echo $(SOURCE_DATE_EPOCH); \
	cat $(PACKAGE_DIR_ALL)/packages.adb $(TOPDIR)/.config $(STAGING_DIR_HOST)/bin/apk \
		$(STAGING_DIR_HOST)/bin/tar $(STAGING_DIR_HOST)/bin/gzip \
		$(if $(CONFIG_USE_MKLIBS),$(SCRIPT_DIR)/rstrip.sh) \
		$(INCLUDE_DIR)/rootfs.mk $(TOPDIR)/package/Makefile; \
	[ ! -d $(TOPDIR)/files ] || ( cd $(TOPDIR)/files && \
		find . -printf '%p %y %m %l\n' | LC_ALL=C sort && \
		find . -type f -print0 | LC_ALL=C sort -z | xargs -0 -r cat ); \
) | $(MKHASH) sha256)
endef

define rootfs_cache_restore
	cp -a $(ROOTFS_CACHE)/root $(TARGET_DIR)
	cp -a $(ROOTFS_CACHE)/root.orig $(TARGET_DIR_ORIG)
endef

APK_INSTALL_ARGS = \
	$(foreach pkg,$(shell cat $(PACKAGE_INSTALL_FILES) 2>/dev/null),$(pkg)$(call GetABISuffix,$(pkg))) \
	base-files=$(shell cat $(STAGING_DIR)/base-files.version) \
	libc=$(shell cat $(STAGING_DIR)/libc.version) \
	kernel=$(subst -rc,_rc,$(shell cat $(STAGING_DIR)/kernel.version))

define apk_install_rootfs
	mkdir -p $(TARGET_DIR)/tmp
	$(file >$(TMP_DIR)/apk_install_list,$(APK_INSTALL_ARGS))
	$(call apk,$(TARGET_DIR)) add --no-cache --initdb --no-scripts --arch $(ARCH_PACKAGES) \
		--repositories-file /dev/null --repository file://$(PACKAGE_DIR_ALL)/packages.adb \
		$(if $(CONFIG_SIGNED_PACKAGES),,--allow-untrusted) \
		$$(cat $(TMP_DIR)/apk_install_list)
	rm -rf $(TARGET_DIR)/run
	$(CP) $(TARGET_DIR) $(TARGET_DIR_ORIG)
	$(call prepare_rootfs,$(TARGET_DIR),$(TOPDIR)/files)
	rm -rf $(ROOTFS_CACHE)
	mkdir -p $(ROOTFS_CACHE)
	cp -a $(TARGET_DIR) $(ROOTFS_CACHE)/root
	cp -a $(TARGET_DIR_ORIG) $(ROOTFS_CACHE)/root.orig
	echo $(ROOTFS_KEY) > $(ROOTFS_CACHE)/key
endef

$(curdir)/install: $(TMP_DIR)/.build $(curdir)/merge $(curdir)/merge-index
	- find $(STAGING_DIR_ROOT) -type d | $(XARGS) chmod 0755
	rm -rf $(TARGET_DIR) $(TARGET_DIR_ORIG)
ifneq ($(CONFIG_USE_APK),)
	$(eval ROOTFS_KEY:=$(call rootfs_cache_key,$(APK_INSTALL_ARGS)))
	$(if $(filter $(ROOTFS_KEY),$(shell cat $(ROOTFS_CACHE)/key 2>/dev/null)),$(rootfs_cache_restore),$(apk_install_rootfs))
else
	mkdir -p $(TARGET_DIR)/tmp
	$(file >$(TMP_DIR)/opkg_install_list,\
	  $(call opkg_package_files,\
	    $(foreach pkg,$(shell cat $(PACKAGE_INSTALL_FILES) 2>/dev/null),$(pkg)$(call GetABISuffix,$(pkg)))))
	$(call opkg,$(TARGET_DIR)) install $$(cat $(TMP_DIR)/opkg_install_list)
	@for file in $(PACKAGE_INSTALL_FILES); do \
		[ -s $$file.flags ] || continue; \
		for flag in `cat $$file.flags`; do \
			$(call opkg,$(TARGET_DIR)) flag $$flag `cat $$file`; \
		done; \
	done || true
	$(CP) $(TARGET_DIR) $(TARGET_DIR_ORIG)
	$(call prepare_rootfs,$(TARGET_DIR),$(TOPDIR)/files)
endif

$(curdir)/index: FORCE
	@echo Generating package index...
ifneq ($(CONFIG_USE_APK),)
	@for d in $(PACKAGE_SUBDIRS); do \
		set -e; \
		mkdir -p $$d; \
		cd $$d || continue; \
		ls *.apk >/dev/null 2>&1 || continue; \
		if [ -f packages.adb ] && [ -f index.json ] && \
		   [ -z "$$(find . -maxdepth 1 \( -name '*.apk' -o -type d \) \
				-newer index.json -print -quit 2>/dev/null)" ]; then \
			continue; \
		fi; \
		$(STAGING_DIR_HOST)/bin/apk mkndx \
			--root $(TOPDIR) \
			--keys-dir $(TOPDIR) \
			--allow-untrusted \
			$(if $(CONFIG_SIGNED_PACKAGES),--sign $(BUILD_KEY_APK_SEC),) \
			--output packages.adb \
			*.apk; \
		$(STAGING_DIR_HOST)/bin/apk adbdump --format json packages.adb | \
			$(SCRIPT_DIR)/make-index-json.py -f apk -a "$(ARCH_PACKAGES)" - > index.json; \
	done
ifdef CONFIG_JSON_CYCLONEDX_SBOM
	@echo Creating CycloneDX package SBOMs...
	@for d in $(PACKAGE_SUBDIRS); do \
		[ -d $$d ] && \
			cd $$d || continue; \
		[ -f packages.adb ] || continue; \
		$(STAGING_DIR_HOST)/bin/apk adbdump --format json packages.adb | \
			$(SCRIPT_DIR)/make-sbom.py - -f apk > Packages.bom.cdx.json || true; \
	done
endif
else
	@for d in $(PACKAGE_SUBDIRS); do ( \
		mkdir -p $$d; \
		cd $$d || continue; \
		$(SCRIPT_DIR)/ipkg-make-index.sh . 2>&1 > Packages.manifest; \
		grep -vE '^(Maintainer|LicenseFiles|Source|SourceName|Require|SourceDateEpoch)' Packages.manifest > Packages; \
		case "$$(((64 + $$(stat -L -c%s Packages)) % 128))" in 110|111) \
			$(call ERROR_MESSAGE,WARNING: Applying padding in $$d/Packages to workaround usign SHA-512 bug!); \
			{ echo ""; echo ""; } >> Packages;; \
		esac; \
		$(SCRIPT_DIR)/make-index-json.py -f opkg -a "$(ARCH_PACKAGES)" Packages > index.json; \
		gzip -9nc Packages > Packages.gz; \
	); done
ifdef CONFIG_SIGNED_PACKAGES
	@echo Signing package index...
	@for d in $(PACKAGE_SUBDIRS); do ( \
		[ -d $$d ] && \
			cd $$d || continue; \
		$(STAGING_DIR_HOST)/bin/usign -S -m Packages -s $(BUILD_KEY); \
	); done
endif
ifdef CONFIG_JSON_CYCLONEDX_SBOM
	@echo Creating CycloneDX package SBOMs...
	@for d in $(PACKAGE_SUBDIRS); do ( \
		[ -d $$d ] && \
			cd $$d || continue; \
		$(SCRIPT_DIR)/package-metadata.pl pkgcyclonedxsbom Packages.manifest > Packages.bom.cdx.json || true; \
	); done
endif
endif
	$(call sha256sums,$(OUTPUT_DIR)/packages/$(ARCH_PACKAGES),1)

$(curdir)/flags-install:= -j1

$(eval $(call stampfile,$(curdir),package,prereq,.config))
$(eval $(call stampfile,$(curdir),package,cleanup,.config $(TMP_DIR)/.packageinfo))
$(eval $(call stampfile,$(curdir),package,compile,$(TMP_DIR)/.build))
$(eval $(call stampfile,$(curdir),package,install,$(TMP_DIR)/.build))
$(eval $(call stampfile,$(curdir),package,check,$(TMP_DIR)/.build))

$(eval $(call subdir,$(curdir)))

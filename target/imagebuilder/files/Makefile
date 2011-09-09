# Makefile for OpenWrt
#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

TOPDIR:=${CURDIR}
LC_ALL:=C
LANG:=C
export TOPDIR LC_ALL LANG
export KBUILD_VERBOSE=99
all: help

include $(TOPDIR)/include/host.mk

ifneq ($(OPENWRT_BUILD),1)
  override OPENWRT_BUILD=1
  export OPENWRT_BUILD
endif

include rules.mk
include $(INCLUDE_DIR)/debug.mk
include $(INCLUDE_DIR)/depends.mk

include $(INCLUDE_DIR)/version.mk
export REVISION

define Helptext
Available Commands:
	help:	This help text
	info:	Show a list of available target profiles
	clean:	Remove images and temporary build files
	image:	Build an image (see below for more information).

Building images:
	By default 'make image' will create an image with the default
	target profile and package set. You can use the following parameters
	to change that:
	
	make image PROFILE="<profilename>" # override the default target profile
	make image PACKAGES="<pkg1> [<pkg2> [<pkg3> ...]]" # include extra packages
	make image FILES="<path>" # include extra files from <path>
	make image BIN_DIR="<path>" # alternative output directory for the images

endef
$(eval $(call shexport,Helptext))

help: FORCE
	echo "$$$(call shvar,Helptext)"


# override variables from rules.mk
PACKAGE_DIR:=$(TOPDIR)/packages
OPKG:= \
  IPKG_TMP="$(TOPDIR)/tmp/ipkgtmp" \
  IPKG_INSTROOT="$(TARGET_DIR)" \
  IPKG_CONF_DIR="$(TOPDIR)/tmp" \
  IPKG_OFFLINE_ROOT="$(TARGET_DIR)" \
  $(STAGING_DIR_HOST)/bin/opkg \
	-f $(TOPDIR)/repositories.conf \
	--force-depends \
	--force-overwrite \
	--force-postinstall \
	--cache $(TOPDIR)/dl \
	--offline-root $(TARGET_DIR) \
	--add-dest root:/ \
	--add-arch all:100 \
	--add-arch $(ARCH_PACKAGES):200

define Profile
  $(eval $(call Profile/Default))
  $(eval $(call Profile/$(1)))
  ifeq ($(USER_PROFILE),)
    USER_PROFILE:=$(1)
  endif
  $(1)_NAME:=$(NAME)
  $(1)_PACKAGES:=$(PACKAGES)
  PROFILE_LIST += \
  	echo '$(1):'; [ -z '$(NAME)' ] || echo '	$(NAME)'; echo '	Packages: $(PACKAGES)';
endef

include $(INCLUDE_DIR)/target.mk

_call_info: FORCE
	echo 'Current Target: "$(BOARD)$(if $(SUBTARGET), ($(BOARDNAME)))"'
	echo 'Default Packages: $(DEFAULT_PACKAGES)'
	echo 'Available Profiles:'
	echo; $(PROFILE_LIST)

BUILD_PACKAGES:=$(sort $(DEFAULT_PACKAGES) $(USER_PACKAGES) $($(USER_PROFILE)_PACKAGES) kernel)
# "-pkgname" in the package list means remove "pkgname" from the package list
BUILD_PACKAGES:=$(filter-out $(filter -%,$(BUILD_PACKAGES)) $(patsubst -%,%,$(filter -%,$(BUILD_PACKAGES))),$(BUILD_PACKAGES))
PACKAGES:=

_call_image:
	echo 'Building images for $(BOARD)$(if $($(USER_PROFILE)_NAME), - $($(USER_PROFILE)_NAME))'
	echo 'Packages: $(BUILD_PACKAGES)'
	echo
	rm -rf $(TARGET_DIR)
	mkdir -p $(TARGET_DIR) $(BIN_DIR) $(TMP_DIR)
	if [ ! -f "$(PACKAGE_DIR)/Packages" ] || [ ! -f "$(PACKAGE_DIR)/Packages.gz" ] || [ "`find $(PACKAGE_DIR) -cnewer $(PACKAGE_DIR)/Packages.gz`" ]; then \
		echo "Package list missing or not up-to-date, generating it.";\
		$(MAKE) package_index; \
	else \
		mkdir -p $(TARGET_DIR)/tmp; \
		$(OPKG) update; \
	fi
	$(MAKE) package_install
ifneq ($(USER_FILES),)
	$(MAKE) copy_files
endif
	$(MAKE) package_postinst
	$(MAKE) build_image
	
package_index: FORCE
	@echo
	@echo Building package index...
	@mkdir -p $(TOPDIR)/tmp $(TOPDIR)/dl $(TARGET_DIR)/tmp
	(cd $(PACKAGE_DIR); $(SCRIPT_DIR)/ipkg-make-index.sh . > Packages && \
		gzip -9c Packages > Packages.gz \
	) >/dev/null 2>/dev/null
	$(OPKG) update

package_install: FORCE
	@echo
	@echo Installing packages...
	$(OPKG) install $(BUILD_PACKAGES)
	rm -f $(TARGET_DIR)/usr/lib/opkg/lists/*

copy_files: FORCE
	@echo
	@echo Copying extra files
	$(CP) $(USER_FILES)/* $(TARGET_DIR)/

package_postinst: FORCE
	@echo
	@echo Cleaning up
	@rm -f $(TARGET_DIR)/tmp/opkg.lock
	@echo
	@echo Activating init scripts
	@( \
		cd $(TARGET_DIR); \
		for script in ./etc/init.d/*; do \
			grep '#!/bin/sh /etc/rc.common' $$script >/dev/null || continue; \
			IPKG_INSTROOT=$(TARGET_DIR) $(which bash) ./etc/rc.common $$script enable; \
		done || true; \
	)
	$(if $(CONFIG_CLEAN_IPKG),rm -rf $(TARGET_DIR)/usr/lib/opkg)

build_image: FORCE
	@echo
	@echo Building images...
	$(NO_TRACE_MAKE) -C target/linux/$(BOARD)/image install TARGET_BUILD=1 IB=1
	
clean:
	rm -rf $(TOPDIR)/tmp $(TOPDIR)/dl $(TARGET_DIR) $(BIN_DIR)


info:
	(unset PROFILE FILES PACKAGES MAKEFLAGS; $(MAKE) -s _call_info)

image:
	(unset PROFILE FILES PACKAGES MAKEFLAGS; \
	$(MAKE) _call_image \
		$(if $(PROFILE),USER_PROFILE="$(PROFILE)") \
		$(if $(FILES),USER_FILES="$(FILES)") \
		$(if $(PACKAGES),USER_PACKAGES="$(PACKAGES)") \
		$(if $(BIN_DIR),BIN_DIR="$(BIN_DIR)"))

.SILENT: help info image


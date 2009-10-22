# Makefile for OpenWrt
#
# Copyright (C) 2007-2009 OpenWrt.org
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

endef
$(eval $(call shexport,Helptext))

help: FORCE
	echo "$$$(call shvar,Helptext)"


# override variables from rules.mk
PACKAGE_DIR:=$(TOPDIR)/packages
IPKG:= \
  IPKG_TMP="$(TOPDIR)/tmp/ipkgtmp" \
  IPKG_INSTROOT="$(TARGET_DIR)" \
  IPKG_CONF_DIR="$(TOPDIR)/tmp" \
  IPKG_OFFLINE_ROOT="$(TARGET_DIR)" \
  $(SCRIPT_DIR)/ipkg -force-defaults

define Profile
  $(eval $(call Profile/Default))
  $(eval $(call Profile/$(1)))
  ifeq ($(PROFILE),)
    PROFILE:=$(1)
  endif
  $(1)_NAME:=$(NAME)
  $(1)_PACKAGES:=$(PACKAGES)
  PROFILE_LIST += \
  	echo '$(1):'; [ -z '$(NAME)' ] || echo '	$(NAME)'; echo '	Packages: $(PACKAGES)';
endef

include $(INCLUDE_DIR)/target.mk

info: FORCE
	echo 'Current Target: "$(BOARD)$(if $(SUBTARGET), ($(BOARDNAME)))"'
	echo 'Default Packages: $(DEFAULT_PACKAGES)'
	echo 'Available Profiles:'
	echo; $(PROFILE_LIST)

$(TOPDIR)/tmp/ipkg.conf: FORCE
	@mkdir -p $(TOPDIR)/tmp
	@echo 'dest root /' > $@
	@echo 'src packages file:$(PACKAGE_DIR)' >> $@

BUILD_PACKAGES:=$(sort $(DEFAULT_PACKAGES) $($(PROFILE)_PACKAGES) kernel)
# "-pkgname" in the package list means remove "pkgname" from the package list
BUILD_PACKAGES:=$(filter-out $(filter -%,$(BUILD_PACKAGES)) $(patsubst -%,%,$(filter -%,$(BUILD_PACKAGES))),$(BUILD_PACKAGES))

image:
	echo 'Building images for $(BOARD)$(if $($(PROFILE)_NAME), - $($(PROFILE)_NAME))'
	echo 'Packages: $(BUILD_PACKAGES)'
	echo
	rm -rf $(TARGET_DIR)
	mkdir -p $(TARGET_DIR) $(BIN_DIR) $(TMP_DIR)
	$(MAKE) package_index
	$(MAKE) package_install
ifneq ($(FILES),)
	$(MAKE) copy_files
endif
	$(MAKE) package_postinst
	$(MAKE) build_image
	
package_index: $(TOPDIR)/tmp/ipkg.conf FORCE
	@echo
	@echo Building package index...
	(cd $(PACKAGE_DIR); $(SCRIPT_DIR)/ipkg-make-index.sh . > Packages && \
		gzip -9c Packages > Packages.gz \
	) >/dev/null 2>/dev/null
	$(IPKG) update

package_install: FORCE
	@echo
	@echo Installing packages...
	$(IPKG) install $(BUILD_PACKAGES)

copy_files: FORCE
	@echo
	@echo Copying extra files
	$(CP) $(FILES)/* $(TARGET_DIR)/

package_postinst: FORCE
	@echo
	@echo Activating init scripts
	@( \
		cd $(TARGET_DIR); \
		for script in ./etc/init.d/*; do \
			grep '#!/bin/sh /etc/rc.common' $$script >/dev/null || continue; \
			IPKG_INSTROOT=$(TARGET_DIR) $(which bash) ./etc/rc.common $$script enable; \
		done || true; \
	)

build_image: FORCE
	@echo
	@echo Building images...
	$(NO_TRACE_MAKE) -C target/linux/$(BOARD)/image install TARGET_BUILD=1 IB=1
	
clean:
	rm -rf tmp $(TARGET_DIR) $(BIN_DIR)


.SILENT: help info image


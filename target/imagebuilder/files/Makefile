# Makefile for the OpenWrt Image Builder
#
# Copyright (C) 2006-2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

export TOPDIR=${CURDIR}

all: help

include rules.mk
include .config

SHELL:=/usr/bin/env bash
export LC_ALL=C
export LANG=C
ifeq ($(KBUILD_VERBOSE),99)
  MAKE:=3>/dev/null $(MAKE)
endif
export IS_TTY=$(shell tty -s && echo 1 || echo 0)

# override variables from rules.mk
PACKAGE_DIR:=$(TOPDIR)/packages
IPKG:= \
  IPKG_TMP="$(TOPDIR)/tmp/ipkgtmp" \
  IPKG_INSTROOT="$(TARGET_DIR)" \
  IPKG_CONF_DIR="$(TOPDIR)/tmp" \
  IPKG_OFFLINE_ROOT="$(TARGET_DIR)" \
  $(SCRIPT_DIR)/ipkg -force-defaults


define Profile/Default
  ID:=
  NAME:=
  PACKAGES:=
endef

define Profile
  $(eval $(call Profile/Default))
  $(eval $(call Profile/$(1)))
  ifneq ($(ID),)
    ifeq ($(PROFILE),)
      PROFILE:=$(ID)
    endif
	$(ID)_NAME:=$(NAME)
	$(ID)_PACKAGES:=$(PACKAGES)
    PROFILE_LIST += \
  		echo '$(ID):'; [ -z '$(NAME)' ] || echo '	$(NAME)'; echo '	Packages: $(PACKAGES)';
  endif
endef

include .target.mk

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

info: FORCE
	echo 'Current Target: "$(BOARDNAME)"'
	echo 'Available Profiles:'
	echo; $(PROFILE_LIST)

$(TOPDIR)/tmp/ipkg.conf: FORCE
	@mkdir -p $(TOPDIR)/tmp
	@echo 'dest root /' > $@
	@echo 'src packages file:$(TOPDIR)/packages' >> $@

BUILD_PACKAGES:=$(sort $(DEFAULT_PACKAGES) $(PACKAGES) $($(PROFILE)_PACKAGES) kernel)
BUILD_PACKAGES:=$(patsubst base-files,base-files-$(BOARD)-$(KERNEL),$(BUILD_PACKAGES))
# "-pkgname" in the package list means remove "pkgname" from the package list
BUILD_PACKAGES:=$(filter-out $(filter -%,$(BUILD_PACKAGES)) $(patsubst -%,%,$(filter -%,$(BUILD_PACKAGES))),$(BUILD_PACKAGES))

image:
	if [ -z "$($(PROFILE)_NAME)" ]; then \
		echo Profile $(PROFILE) not found.; \
		echo 'Use "make info" to get a list of available target profiles'; \
		false; \
	fi
	echo 'Building images for $(BOARDNAME) - $($(PROFILE)_NAME)'
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
	(cd $(PACKAGE_DIR); $(SCRIPT_DIR)/ipkg-make-index.sh . > Packages) >/dev/null 2>/dev/null
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
	( \
		cd $(BUILD_DIR)/root; \
		for script in ./etc/init.d/*; do \
			grep '#!/bin/sh /etc/rc.common' $$script >/dev/null || continue; \
			IPKG_INSTROOT=$(BUILD_DIR)/root $(which bash) ./etc/rc.common $$script enable; \
		done; \
	)

build_image: FORCE
	@echo
	@echo Building images...
	$(NO_TRACE_MAKE) -C target/linux/$(BOARD)-$(KERNEL)/image install IB=1
	
clean:
	rm -rf tmp $(TARGET_DIR) $(BIN_DIR)

.PHONY: FORCE
.SILENT: help info image
%: ;

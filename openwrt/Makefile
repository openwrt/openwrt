# Makefile for buildroot2
#
# Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#

#--------------------------------------------------------------
# Just run 'make menuconfig', configure stuff, then run 'make'.
# You shouldn't need to mess with anything beyond this point...
#--------------------------------------------------------------
TOPDIR=./
CONFIG_CONFIG_IN = Config.in
CONFIG_DEFCONFIG = .defconfig
CONFIG = package/config

noconfig_targets := menuconfig config oldconfig randconfig \
	defconfig allyesconfig allnoconfig release tags

# Pull in the user's configuration file
ifeq ($(filter $(noconfig_targets),$(MAKECMDGOALS)),)
-include $(TOPDIR).config
endif

ifeq ($(BR2_TAR_VERBOSITY),y)
TAR_OPTIONS=-xvf
else
TAR_OPTIONS=-xf
endif

ifeq ($(strip $(BR2_HAVE_DOT_CONFIG)),y)

#############################################################
#
# The list of stuff to build for the target toolchain
# along with the packages to build for the target.
#
##############################################################
TARGETS:=host-sed kernel-headers uclibc-configured binutils gcc uclibc-target-utils
TARGETS+=openwrt-sstrip linux

include toolchain/Makefile.in
include package/Makefile.in

#############################################################
#
# You should probably leave this stuff alone unless you know
# what you are doing.
#
#############################################################



all:   world

# In this section, we need .config
include .config.cmd

# We also need the various per-package makefiles, which also add
# each selected package to TARGETS if that package was selected
# in the .config file.
include toolchain/*/*.mk
include package/*/*.mk
include target/*/*.mk

# target stuff is last so it can override anything else
include target/Makefile.in

TARGETS_CLEAN:=$(patsubst %,%-clean,$(TARGETS))
TARGETS_SOURCE:=$(patsubst %,%-source,$(TARGETS))
TARGETS_DIRCLEAN:=$(patsubst %,%-dirclean,$(TARGETS))

world: $(DL_DIR) $(BUILD_DIR) $(STAGING_DIR) $(TARGET_DIR) $(TARGETS)

.PHONY: all world clean dirclean distclean source $(TARGETS) \
	$(TARGETS_CLEAN) $(TARGETS_DIRCLEAN) $(TARGETS_SOURCE) \
	$(DL_DIR) $(BUILD_DIR) $(TOOL_BUILD_DIR) $(STAGING_DIR)

#############################################################
#
# staging and target directories do NOT list these as
# dependancies anywhere else
#
#############################################################
$(DL_DIR):
	@mkdir -p $(DL_DIR)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(TOOL_BUILD_DIR):
	@mkdir -p $(TOOL_BUILD_DIR)

$(STAGING_DIR):
	@mkdir -p $(STAGING_DIR)/lib
	@mkdir -p $(STAGING_DIR)/include
	@mkdir -p $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)
	@ln -sf ../lib $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)/lib

$(TARGET_DIR):
	if [ -f "$(TARGET_SKELETON)" ] ; then \
		zcat $(TARGET_SKELETON) | tar -C $(BUILD_DIR) -xf -; \
	fi;
	if [ -d "$(TARGET_SKEL_DIR)" ] ; then \
		cp -a $(TARGET_SKEL_DIR)/* $(TARGET_DIR)/; \
	fi;
	-find $(TARGET_DIR) -type d -name CVS | xargs rm -rf
	-find $(TARGET_DIR) -type d -name .svn | xargs rm -rf
	-ln -sf /tmp/resolv.conf $(TARGET_DIR)
	-mkdir -p $(TARGET_DIR)/jffs

source: $(TARGETS_SOURCE)

#############################################################
#
# Cleanup and misc junk
#
#############################################################
clean: 
	rm -rf $(TARGET_DIR) $(IMAGE).*
	$(MAKE) openwrt-image-clean

dirclean: $(TARGETS_DIRCLEAN)
	rm -rf $(TARGET_DIR) $(IMAGE).*
	$(MAKE) openwrt-image-dirclean

distclean: clean
	rm -rf $(DL_DIR) $(BUILD_DIR) $(TOOL_BUILD_DIR) 
	rm .config* .tmpconfig.h

sourceball:
	rm -rf $(BUILD_DIR)
	set -e; \
	cd ..; \
	rm -f buildroot.tar.bz2; \
	tar -cvf buildroot.tar buildroot; \
	bzip2 -9 buildroot.tar; \


else # ifeq ($(strip $(BR2_HAVE_DOT_CONFIG)),y)

all: menuconfig

# configuration
# ---------------------------------------------------------------------------

$(CONFIG)/conf:
	$(MAKE) -C $(CONFIG) conf
	-@if [ ! -f .config ] ; then \
		cp $(CONFIG_DEFCONFIG) .config; \
	fi
$(CONFIG)/mconf:
	$(MAKE) -C $(CONFIG) ncurses conf mconf
	-@if [ ! -f .config ] ; then \
		cp $(CONFIG_DEFCONFIG) .config; \
	fi

menuconfig: $(CONFIG)/mconf
	-touch .config
	-cp .config .config.test
	@$(CONFIG)/mconf $(CONFIG_CONFIG_IN)
	-./scripts/configtest.pl

config: $(CONFIG)/conf
	-touch .config
	-cp .config .config.test
	@$(CONFIG)/conf $(CONFIG_CONFIG_IN)
	-./scripts/configtest.pl

oldconfig: $(CONFIG)/conf
	-touch .config
	-cp .config .config.test
	@$(CONFIG)/conf -o $(CONFIG_CONFIG_IN)
	-./scripts/configtest.pl

randconfig: $(CONFIG)/conf
	-touch .config
	-cp .config .config.test
	@$(CONFIG)/conf -r $(CONFIG_CONFIG_IN)
	-./scripts/configtest.pl

allyesconfig: $(CONFIG)/conf
	#@$(CONFIG)/conf -y $(CONFIG_CONFIG_IN)
	#sed -i -e "s/^CONFIG_DEBUG.*/# CONFIG_DEBUG is not set/" .config
	-touch .config
	-cp .config .config.test
	@$(CONFIG)/conf -o $(CONFIG_CONFIG_IN)
	-./scripts/configtest.pl

allnoconfig: $(CONFIG)/conf
	-touch .config
	-cp .config .config.test
	@$(CONFIG)/conf -n $(CONFIG_CONFIG_IN)
	-./scripts/configtest.pl

defconfig: $(CONFIG)/conf
	-touch .config
	-cp .config .config.test
	@$(CONFIG)/conf -d $(CONFIG_CONFIG_IN)
	-./scripts/configtest.pl

#############################################################
#
# Cleanup and misc junk
#
#############################################################
clean:
	@$(MAKE) -C $(CONFIG) clean

distclean: clean

endif # ifeq ($(strip $(BR2_HAVE_DOT_CONFIG)),y)

.PHONY: dummy subdirs release distclean clean config oldconfig \
	menuconfig tags check test depend


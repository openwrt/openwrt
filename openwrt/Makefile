# Makefile for OpenWrt
#
# Copyright (C) 2006 OpenWrt.org
# Copyright (C) 2006 by Felix Fietkau <openwrt@nbd.name>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

RELEASE:=Kamikaze
#VERSION:=2.0 # uncomment for final release

#--------------------------------------------------------------
# Just run 'make menuconfig', configure stuff, then run 'make'.
# You shouldn't need to mess with anything beyond this point...
#--------------------------------------------------------------

all: world

export TOPDIR=${shell pwd}
include $(TOPDIR)/include/verbose.mk

OPENWRTVERSION:=$(RELEASE)
ifneq ($(VERSION),)
  OPENWRTVERSION:=$(VERSION) ($(OPENWRTVERSION))
else
  REV:=$(shell LANG=C svn info | awk '/^Revision:/ { print$$2 }' )
  ifneq ($(REV),)
    OPENWRTVERSION:=$(OPENWRTVERSION)/r$(REV)
  endif
endif
export OPENWRTVERSION

ifneq ($(shell ./scripts/timestamp.pl -p .pkginfo package Makefile),.pkginfo)
  .pkginfo .config: FORCE
endif

ifeq ($(FORCE),)
  .config scripts/config/conf scripts/config/mconf: .prereq-build
  world: .prereq-packages
endif

.pkginfo:
	@echo Collecting package info...
	@-for dir in package/*/; do \
		echo Source-Makefile: $${dir}Makefile; \
		$(NO_TRACE_MAKE) --no-print-dir DUMP=1 -C $$dir 2>&- || echo "ERROR: please fix package/$${dir}Makefile" >&2; \
	done > $@

pkginfo-clean: FORCE
	-rm -f .pkginfo .config.in

.config.in: .pkginfo
	@./scripts/gen_menuconfig.pl < $< > $@ || rm -f $@

.config: ./scripts/config/conf
	@[ -f .config ] || $(NO_TRACE_MAKE) menuconfig
	@$< -D .config Config.in &> /dev/null

scripts/config/mconf:
	@$(MAKE) -C scripts/config all

scripts/config/conf:
	@$(MAKE) -C scripts/config conf

config: scripts/config/conf .config.in FORCE
	$< Config.in

config-clean: FORCE
	$(NO_TRACE_MAKE) -C scripts/config clean

defconfig: scripts/config/conf .config.in FORCE
	touch .config
	$< -D .config Config.in

oldconfig: scripts/config/conf .config.in FORCE
	$< -o Config.in

menuconfig: scripts/config/mconf .config.in FORCE
	$< Config.in

package/%: .pkginfo FORCE
	$(MAKE) -C package $(patsubst package/%,%,$@)

target/%: .pkginfo FORCE
	$(MAKE) -C target $(patsubst target/%,%,$@)

toolchain/%: FORCE
	$(MAKE) -C toolchain $(patsubst toolchain/%,%,$@)

.prereq-build: include/prereq-build.mk
	@$(NO_TRACE_MAKE) -s -f $(TOPDIR)/include/prereq-build.mk prereq 2>/dev/null || { \
		echo "Prerequisite check failed. Use FORCE=1 to override."; \
		false; \
	}
	@touch $@

.prereq-packages: include/prereq.mk .pkginfo .config
	@$(NO_TRACE_MAKE) -s -C package prereq 2>/dev/null || { \
		echo "Prerequisite check failed. Use FORCE=1 to override."; \
		false; \
	}
	@touch $@
	
prereq: .prereq-build .prereq-packages FORCE

download: .config FORCE
	$(MAKE) toolchain/download
	$(MAKE) package/download
	$(MAKE) target/download

world: .config FORCE
	$(MAKE) toolchain/install
	$(MAKE) target/compile
	$(MAKE) package/compile
	$(MAKE) package/install
	$(MAKE) target/install
	$(MAKE) package/index

clean: FORCE
	rm -rf build_* bin

dirclean: clean
	rm -rf staging_dir_* toolchain_build_*

distclean: dirclean config-clean
	rm -rf dl .*config* .pkg* .prereq

.SILENT: clean dirclean distclean config-clean download world
FORCE: ;
.PHONY: FORCE
%: ;

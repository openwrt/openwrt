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
TOPDIR=${shell pwd}
export TOPDIR
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

all: world

.pkginfo: FORCE
ifneq ($(shell ./scripts/timestamp.pl -p .pkginfo package Makefile),.pkginfo)
	@echo Collecting package info...
	@-for dir in package/*/; do \
		echo Source-Makefile: $${dir}Makefile; \
		$(NO_TRACE_MAKE) --no-print-dir DUMP=1 -C $$dir 2>&- || true; \
	done > $@
endif

.config.in: .pkginfo
	./scripts/gen_menuconfig.pl < $< > $@ || rm -f $@

pkginfo-clean: FORCE
	-rm -f .pkginfo .config.in

./scripts/config/mconf: .config.in
	$(MAKE) -C scripts/config all

./scripts/config/conf: .config.in
	$(MAKE) -C scripts/config conf

config: ./scripts/config/conf FORCE
	$< Config.in

defconfig: ./scripts/config/conf FORCE
	touch .config
	$< -D .config Config.in

oldconfig: ./scripts/config/conf FORCE
	$< -o Config.in

menuconfig: ./scripts/config/mconf FORCE
	$< Config.in

config-clean: FORCE
	$(MAKE) -C scripts/config clean

package/%: .pkginfo FORCE
	$(MAKE) -C package $(patsubst package/%,%,$@)

target/%: .pkginfo FORCE
	$(MAKE) -C target $(patsubst target/%,%,$@)

toolchain/%: FORCE
	$(MAKE) -C toolchain $(patsubst toolchain/%,%,$@)

.config: ./scripts/config/conf FORCE
	@[ -f .config ] || $(NO_TRACE_MAKE) menuconfig
	$< -D .config Config.in >/dev/null 2>/dev/null

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

clean: FORCE
	rm -rf build_* bin

dirclean: clean
	rm -rf staging_dir_* toolchain_build_*

distclean: dirclean config-clean
	rm -rf dl .*config* .pkg*

.PHONY: FORCE
FORCE:

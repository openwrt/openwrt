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

SHELL:=/usr/bin/env bash
export LC_ALL=C
export LANG=C
export TOPDIR=${CURDIR}
ifeq ($(KBUILD_VERBOSE),99)
  MAKE:=3>/dev/null $(MAKE)
endif
export IS_TTY=$(shell tty -s && echo 1 || echo 0)

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

ifeq ($(FORCE),)
  .config scripts/config/conf scripts/config/mconf: tmp/.prereq-build
  world: tmp/.prereq-package tmp/.prereq-target
endif

package/%/Makefile: ;
target/%/Makefile: ;

tmp/.packageinfo: FORCE
tmp/.targetinfo: FORCE
tmp/.%info:
	mkdir -p tmp/info
	$(NO_TRACE_MAKE) -s -f include/scan.mk SCAN_TARGET="$*info" SCAN_DIR="$(patsubst target,target/linux,$*)" SCAN_NAME="$*" SCAN_DEPS="$^" SCAN_EXTRA=""

tmpinfo-clean: FORCE
	-rm -rf tmp/.*info

tmp/.config-%.in: tmp/.%info
	./scripts/metadata.pl $*_config < $< > $@ || rm -f $@



.config: ./scripts/config/conf tmp/.config-target.in tmp/.config-package.in
	if [ \! -f .config -a -e $(HOME)/.openwrt/defconfig ]; then \
		cp $(HOME)/.openwrt/defconfig .config; \
		$(NO_TRACE_MAKE) menuconfig; \
	fi
	$< -D .config Config.in &> /dev/null

scripts/config/mconf:
	$(MAKE) -C scripts/config all

scripts/config/conf:
	$(MAKE) -C scripts/config conf



config: scripts/config/conf tmp/.config-target.in tmp/.config-package.in FORCE
	$< Config.in

config-clean: FORCE
	$(NO_TRACE_MAKE) -C scripts/config clean

defconfig: scripts/config/conf tmp/.config-target.in tmp/.config-package.in FORCE
	touch .config
	$< -D .config Config.in

oldconfig: scripts/config/conf tmp/.config-target.in tmp/.config-package.in FORCE
	$< -o Config.in

menuconfig: scripts/config/mconf tmp/.config-target.in tmp/.config-package.in FORCE
	if [ \! -f .config -a -e $(HOME)/.openwrt/defconfig ]; then \
		cp $(HOME)/.openwrt/defconfig .config; \
	fi
	$< Config.in

kernel_menuconfig: .config FORCE
	-$(MAKE) target/linux-prepare
	$(NO_TRACE_MAKE) -C target/linux menuconfig


package/% target/%: tmp/.packageinfo
toolchain/% package/% target/%: tmp/.targetinfo
package/% target/% tools/% toolchain/%: FORCE
	$(MAKE) -C $(patsubst %/$*,%,$@) $*


tmp/.prereq-build: include/prereq-build.mk
	mkdir -p tmp
	rm -f tmp/.host.mk
	$(NO_TRACE_MAKE) -s -f $(TOPDIR)/include/prereq-build.mk prereq 2>/dev/null || { \
		echo "Prerequisite check failed. Use FORCE=1 to override."; \
		false; \
	}
	touch $@

tmp/.prereq-%: include/prereq.mk tmp/.%info .config
	mkdir -p tmp
	rm -f tmp/.host.mk
	$(NO_TRACE_MAKE) -s -C $* prereq 2>/dev/null || { \
		echo "Prerequisite check failed. Use FORCE=1 to override."; \
		false; \
	}
	touch $@

prereq: tmp/.prereq-build tmp/.prereq-package tmp/.prereq-target FORCE

download: .config FORCE
	$(MAKE) tools/download
	$(MAKE) toolchain/download
	$(MAKE) package/download
	$(MAKE) target/download

world: .config FORCE
	$(MAKE) tools/install
	$(MAKE) toolchain/install
	$(MAKE) target/compile
	$(MAKE) package/compile
	$(MAKE) package/install
	$(MAKE) target/install
	$(MAKE) package/index

clean: FORCE
	rm -rf build_* bin tmp

dirclean: clean
	rm -rf staging_dir_* toolchain_build_* tool_build

distclean: dirclean config-clean symlinkclean docclean
	rm -rf dl

help:
	cat README

doc:
	$(MAKE) -C docs/ openwrt.pdf

docclean:
	$(MAKE) -C docs/ clean

symlinkclean:
	find package -type l -exec rm -f {} +

.SILENT: clean dirclean distclean symlinkclean config-clean download world help tmp/.%info tmpinfo-clean tmp/.config-%.in .config scripts/config/mconf scripts/config/conf menuconfig tmp/.prereq-build tmp/.prereq-%
FORCE: ;
.PHONY: FORCE help

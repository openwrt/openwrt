# Makefile for OpenWrt
#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

RELEASE:=Attitude Adjustment
PREP_MK= OPENWRT_BUILD= QUIET=0

include $(TOPDIR)/include/verbose.mk

ifeq ($(SDK),1)
  include $(TOPDIR)/include/version.mk
else
  REVISION:=$(shell $(TOPDIR)/scripts/getver.sh)
endif

OPENWRTVERSION:=$(RELEASE)$(if $(REVISION), ($(REVISION)))
export RELEASE
export REVISION
export OPENWRTVERSION
export IS_TTY=$(shell tty -s && echo 1 || echo 0)
export LD_LIBRARY_PATH:=$(if $(LD_LIBRARY_PATH),$(LD_LIBRARY_PATH):)$(STAGING_DIR_HOST)/lib
export DYLD_LIBRARY_PATH:=$(if $(DYLD_LIBRARY_PATH),$(DYLD_LIBRARY_PATH):)$(STAGING_DIR_HOST)/lib

# prevent perforce from messing with the patch utility
unexport P4PORT P4USER P4CONFIG P4CLIENT

# prevent user defaults for quilt from interfering
unexport QUILT_PATCHES QUILT_PATCH_OPTS

# make sure that a predefined CFLAGS variable does not disturb packages
export CFLAGS=

ifeq ($(FORCE),)
  .config scripts/config/conf scripts/config/mconf: tmp/.prereq-build
endif

SCAN_COOKIE?=$(shell echo $$$$)
export SCAN_COOKIE

SUBMAKE:=umask 022; $(SUBMAKE)

prepare-mk: FORCE ;

prepare-tmpinfo: FORCE
	mkdir -p tmp/info
	$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r -s -f include/scan.mk SCAN_TARGET="packageinfo" SCAN_DIR="package" SCAN_NAME="package" SCAN_DEPS="$(TOPDIR)/include/package*.mk $(TOPDIR)/overlay/*/*.mk" SCAN_DEPTH=5 SCAN_EXTRA=""
	$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r -s -f include/scan.mk SCAN_TARGET="targetinfo" SCAN_DIR="target/linux" SCAN_NAME="target" SCAN_DEPS="profiles/*.mk $(TOPDIR)/include/kernel*.mk $(TOPDIR)/include/target.mk" SCAN_DEPTH=2 SCAN_EXTRA="" SCAN_MAKEOPTS="TARGET_BUILD=1"
	for type in package target; do \
		f=tmp/.$${type}info; t=tmp/.config-$${type}.in; \
		[ "$$t" -nt "$$f" ] || ./scripts/metadata.pl $${type}_config "$$f" > "$$t" || { rm -f "$$t"; echo "Failed to build $$t"; false; break; }; \
	done
	./scripts/metadata.pl package_mk tmp/.packageinfo > tmp/.packagedeps || { rm -f tmp/.packagedeps; false; }
	touch $(TOPDIR)/tmp/.build

.config: ./scripts/config/conf $(if $(CONFIG_HAVE_DOT_CONFIG),,prepare-tmpinfo)
	@+if [ \! -e .config ] || ! grep CONFIG_HAVE_DOT_CONFIG .config >/dev/null; then \
		[ -e $(HOME)/.openwrt/defconfig ] && cp $(HOME)/.openwrt/defconfig .config; \
		$(_SINGLE)$(NO_TRACE_MAKE) menuconfig $(PREP_MK); \
	fi

scripts/config/mconf:
	@$(_SINGLE)$(SUBMAKE) -s -C scripts/config all

$(eval $(call rdep,scripts/config,scripts/config/mconf))

scripts/config/conf:
	@$(_SINGLE)$(SUBMAKE) -s -C scripts/config conf

config: scripts/config/conf prepare-tmpinfo FORCE
	$< Config.in

config-clean: FORCE
	$(_SINGLE)$(NO_TRACE_MAKE) -C scripts/config clean

defconfig: scripts/config/conf prepare-tmpinfo FORCE
	touch .config
	$< -D .config Config.in

oldconfig: scripts/config/conf prepare-tmpinfo FORCE
	$< -$(if $(CONFDEFAULT),$(CONFDEFAULT),o) Config.in

menuconfig: scripts/config/mconf prepare-tmpinfo FORCE
	if [ \! -e .config -a -e $(HOME)/.openwrt/defconfig ]; then \
		cp $(HOME)/.openwrt/defconfig .config; \
	fi
	$< Config.in

prepare_kernel_conf: .config FORCE

ifeq ($(wildcard staging_dir/host/bin/sed),)
  prepare_kernel_conf:
	@+$(SUBMAKE) -r tools/sed/install
else
  prepare_kernel_conf: ;
endif

kernel_oldconfig: prepare_kernel_conf
	$(_SINGLE)$(NO_TRACE_MAKE) -C target/linux oldconfig

kernel_menuconfig: prepare_kernel_conf
	$(_SINGLE)$(NO_TRACE_MAKE) -C target/linux menuconfig

kernel_nconfig: prepare_kernel_conf
	$(_SINGLE)$(NO_TRACE_MAKE) -C target/linux nconfig

tmp/.prereq-build: include/prereq-build.mk
	mkdir -p tmp
	rm -f tmp/.host.mk
	@$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r -s -f $(TOPDIR)/include/prereq-build.mk prereq 2>/dev/null || { \
		echo "Prerequisite check failed. Use FORCE=1 to override."; \
		false; \
	}
	touch $@

printdb: FORCE
	@$(_SINGLE)$(NO_TRACE_MAKE) -p $@ V=99 DUMP_TARGET_DB=1 2>&1

download: .config FORCE
	@+$(SUBMAKE) tools/download
	@+$(SUBMAKE) toolchain/download
	@+$(SUBMAKE) package/download
	@+$(SUBMAKE) target/download

clean dirclean: .config
	@+$(SUBMAKE) -r $@ 

prereq:: prepare-tmpinfo .config
	@+$(MAKE) -r -s tmp/.prereq-build $(PREP_MK)
	@+$(NO_TRACE_MAKE) -r -s $@

%::
	@( \
		cp .config tmp/.config; \
		./scripts/config/conf -D tmp/.config -w tmp/.config Config.in > /dev/null 2>&1; \
		if ./scripts/kconfig.pl '>' .config tmp/.config | grep -q CONFIG; then \
			echo "WARNING: your configuration is out of sync. Please run make menuconfig, oldconfig or defconfig!"; \
		fi \
	)
	@+$(PREP_MK) $(NO_TRACE_MAKE) -r -s prereq
	@+$(SUBMAKE) -r $@

help:
	cat README

docs docs/compile: FORCE
	@$(_SINGLE)$(SUBMAKE) -C docs compile

docs/clean: FORCE
	@$(_SINGLE)$(SUBMAKE) -C docs clean

distclean:
	rm -rf tmp build_dir staging_dir dl .config* feeds package/feeds package/openwrt-packages bin
	@$(_SINGLE)$(SUBMAKE) -C scripts/config clean

ifeq ($(findstring v,$(DEBUG)),)
  .SILENT: symlinkclean clean dirclean distclean config-clean download help tmpinfo-clean .config scripts/config/mconf scripts/config/conf menuconfig tmp/.prereq-build tmp/.prereq-package prepare-tmpinfo
endif
.PHONY: help FORCE
.NOTPARALLEL:


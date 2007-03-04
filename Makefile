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
  world: tmp/.prereq-packages tmp/.prereq-target
endif

define stamp
tmp/info/.stamp-$(1)-$(shell ls $(2)/*/Makefile | (md5sum || md5) 2>/dev/null | cut -d' ' -f1)
endef

STAMP_pkginfo=$(call stamp,pkginfo,package)
STAMP_targetinfo=$(call stamp,targetinfo,target/linux)
define scan_info

$(STAMP_$(1)):
	@mkdir -p tmp/info
	@rm -f tmp/info/.stamp-$(1)*
	@touch $$@

$(foreach FILE,$(shell ls $(2)/*/Makefile),
tmp/.$(1): $(FILE)
$(FILE):
)

tmp/.$(1): $(STAMP_$(1)) $(4)
	@echo -n Collecting $(3) info... 
	@$(NO_TRACE_MAKE) -s -f include/scan.mk SCAN_TARGET="$(1)" SCAN_DIR="$(2)" SCAN_NAME="$(3)" SCAN_DEPS="$(4)"

endef

$(eval $(call scan_info,pkginfo,package,package,include/package.mk))
$(eval $(call scan_info,targetinfo,target/linux,target,include/kernel-build.mk include/kernel-version.mk))

tmpinfo-clean: FORCE
	@-rm -rf tmp/.pkginfo tmp/.targetinfo

tmp/.config.in: tmp/.pkginfo
	@./scripts/metadata.pl package_config < $< > $@ || rm -f $@

tmp/.config-target.in: tmp/.targetinfo
	@./scripts/metadata.pl target_config < $< > $@ || rm -f $@

.config: ./scripts/config/conf tmp/.config.in tmp/.config-target.in
	@[ -f .config ] || $(NO_TRACE_MAKE) menuconfig
	@$< -D .config Config.in &> /dev/null

scripts/config/mconf:
	@$(MAKE) -C scripts/config all

scripts/config/conf:
	@$(MAKE) -C scripts/config conf

config: scripts/config/conf tmp/.config.in tmp/.config-target.in FORCE
	$< Config.in

config-clean: FORCE
	$(NO_TRACE_MAKE) -C scripts/config clean

defconfig: scripts/config/conf tmp/.config.in tmp/.config-target.in FORCE
	touch .config
	$< -D .config Config.in

oldconfig: scripts/config/conf tmp/.config.in tmp/.config-target.in FORCE
	$< -o Config.in

menuconfig: scripts/config/mconf tmp/.config.in tmp/.config-target.in FORCE
	$< Config.in

kernel_menuconfig: .config FORCE
	-$(MAKE) target/linux-prepare
	$(NO_TRACE_MAKE) -C target/linux menuconfig

package/%: 
	@$(NO_TRACE_MAKE) -s tmp/.pkginfo tmp/.targetinfo
	$(MAKE) -C package $(patsubst package/%,%,$@)

target/%:
	@$(NO_TRACE_MAKE) -s tmp/.pkginfo tmp/.targetinfo
	$(MAKE) -C target $(patsubst target/%,%,$@)

tools/%: FORCE
	$(MAKE) -C tools $(patsubst tools/%,%,$@)

toolchain/%: tmp/.targetinfo FORCE
	$(MAKE) -C toolchain $(patsubst toolchain/%,%,$@)

tmp/.prereq-build: include/prereq-build.mk
	@mkdir -p tmp
	@rm -f tmp/.host.mk
	@$(NO_TRACE_MAKE) -s -f $(TOPDIR)/include/prereq-build.mk prereq 2>/dev/null || { \
		echo "Prerequisite check failed. Use FORCE=1 to override."; \
		false; \
	}
	@touch $@

tmp/.prereq-packages: include/prereq.mk tmp/.pkginfo .config
	@mkdir -p tmp
	@rm -f tmp/.host.mk
	@$(NO_TRACE_MAKE) -s -C package prereq 2>/dev/null || { \
		echo "Prerequisite check failed. Use FORCE=1 to override."; \
		false; \
	}
	@touch $@

tmp/.prereq-target: include/prereq.mk tmp/.targetinfo .config
	@mkdir -p tmp
	@rm -f tmp/.host.mk
	@$(NO_TRACE_MAKE) -s -C target prereq 2>/dev/null || { \
		echo "Prerequisite check failed. Use FORCE=1 to override."; \
		false; \
	}
	@touch $@

prereq: tmp/.prereq-build tmp/.prereq-packages tmp/.prereq-target FORCE

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

distclean: dirclean config-clean
	rm -rf dl

help:
	cat README

doc:
	$(MAKE) -C docs/ openwrt.pdf

docclean:
	$(MAKE) -C docs/ clean

.SILENT: clean dirclean distclean config-clean download world
FORCE: ;
.PHONY: FORCE help
%: ;

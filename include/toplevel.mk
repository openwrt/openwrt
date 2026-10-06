# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2007-2020 OpenWrt.org

PREP_MK= OPENWRT_BUILD= QUIET=0

export IS_TTY=$(if $(MAKE_TERMOUT),1,0)

include $(TOPDIR)/include/verbose.mk

ifeq ($(SDK),1)
  include $(TOPDIR)/include/version.mk
else ifeq ($(filter-out 0,$(MAKELEVEL))$(origin REVISION)$(origin SOURCE_DATE_EPOCH),$(MAKELEVEL)environmentenvironment)
  # Recursive calls of the top-level Makefile get both from its environment.
else
  REVISION:=$(shell $(TOPDIR)/scripts/getver.sh)
  SOURCE_DATE_EPOCH:=$(shell $(TOPDIR)/scripts/get_source_date_epoch.sh)
endif

export REVISION
export SOURCE_DATE_EPOCH
export GIT_CONFIG_PARAMETERS='core.autocrlf=false'
export GIT_ASKPASS:=/bin/true
export MAKE_JOBSERVER=$(filter --jobserver%,$(MAKEFLAGS))
ifneq ($(filter-out 0,$(MAKELEVEL))$(origin GNU_HOST_NAME),$(MAKELEVEL)environment)
  GNU_HOST_NAME:=$(shell $(TOPDIR)/scripts/config.guess)
endif
export GNU_HOST_NAME
export HOST_OS:=$(shell uname)
export HOST_ARCH:=$(shell uname -m)

ifeq ($(HOST_OS),Darwin)
  ifneq ($(filter /Applications/Xcode.app/% /Library/Developer/%,$(MAKE)),)
    $(error Please use a newer version of GNU make. The version shipped by Apple is not supported)
  endif
endif

# prevent perforce from messing with the patch utility
unexport P4PORT P4USER P4CONFIG P4CLIENT

# prevent user defaults for quilt from interfering
unexport QUILT_PATCHES QUILT_PATCH_OPTS

unexport C_INCLUDE_PATH CROSS_COMPILE ARCH

# prevent distro default LPATH from interfering
unexport LPATH

# make sure that a predefined CFLAGS variable does not disturb packages
export CFLAGS=
export LDFLAGS=

empty:=
space:= $(empty) $(empty)
path:=$(subst :,$(space),$(PATH))
path:=$(filter-out .%,$(path))
path:=$(subst $(space),:,$(path))
export ORIG_PATH:=$(if $(ORIG_PATH),$(ORIG_PATH),$(PATH))
export PATH:=$(path)
export STAGING_DIR_HOST:=$(if $(STAGING_DIR),$(abspath $(STAGING_DIR)/../host),$(TOPDIR)/staging_dir/host)

unexport TAR_OPTIONS

ifeq ($(FORCE),)
  .config scripts/config/conf scripts/config/mconf: $(STAGING_DIR_HOST)/.prereq-build
endif

SCAN_COOKIE?=$(shell echo $$$$)
export SCAN_COOKIE

SUBMAKE:=umask 022; $(SUBMAKE)

ULIMIT_FIX=_limit=`ulimit -n`; [ "$$_limit" = "unlimited" -o "$$_limit" -ge 1024 ] || ulimit -n 1024;

prepare-mk: $(STAGING_DIR_HOST)/.prereq-build FORCE ;

ifdef SDK
  IGNORE_PACKAGES = linux
endif

_ignore = $(foreach p,$(IGNORE_PACKAGES),--ignore $(p))

# package/kernel/linux names these files in SCAN_DEPS, so the package scan
# reads them although they sit in the target tree.
SCAN_DEPS_packageinfo:=target/linux/*/modules.mk target/linux/feeds/*/modules.mk

# $(1): scan target, $(2): scan directory, $(3): scan depth
SCAN_STAMP=tmp/info/.scan-$(1).stamp
scan_unchanged=[ -f $(SCAN_STAMP) ] && \
	[ -z "$$(find -L $(2) -maxdepth $(3) \( -type d -o -name Makefile -o -name '*.mk' \) \
		-newer $(SCAN_STAMP) -print -quit 2>/dev/null)" ] && \
	[ -z "$$(find include rules.mk .config $(SCAN_DEPS_$(1)) -maxdepth 1 \
		\( -name '*.mk' -o -name .config \) \
		-newer $(SCAN_STAMP) -print -quit 2>/dev/null)" ]

# $(1): output file, $(2): package-metadata.pl command
tmpinfo_gen=[ $(1) -nt tmp/.packageinfo ] && [ $(1) -nt scripts/package-metadata.pl ] && \
	[ $(1) -nt scripts/metadata.pm ] || \
	./scripts/package-metadata.pl $(2) tmp/.packageinfo > $(1) || { rm -f $(1); false; }

prepare-tmpinfo: FORCE
	@+$(MAKE) -r -s $(STAGING_DIR_HOST)/.prereq-build $(PREP_MK)
	mkdir -p tmp/info feeds
	[ -e $(TOPDIR)/feeds/base ] || ln -sf ../package $(TOPDIR)/feeds/base
	+$(call scan_unchanged,packageinfo,package,5) || { \
		$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r -s -f include/scan.mk SCAN_TARGET="packageinfo" SCAN_DIR="package" SCAN_NAME="package" SCAN_DEPTH=5 SCAN_EXTRA="" && \
		touch $(call SCAN_STAMP,packageinfo); }
	+$(call scan_unchanged,targetinfo,target/linux,3) || { \
		$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r -s -f include/scan.mk SCAN_TARGET="targetinfo" SCAN_DIR="target/linux" SCAN_NAME="target" SCAN_DEPTH=3 SCAN_EXTRA="" SCAN_MAKEOPTS="TARGET_BUILD=1" && \
		touch $(call SCAN_STAMP,targetinfo); }
	for type in package target; do \
		f=tmp/.$${type}info; t=tmp/.config-$${type}.in; \
		[ "$$t" -nt "$$f" ] || ./scripts/$${type}-metadata.pl $(_ignore) config "$$f" > "$$t" || { rm -f "$$t"; echo "Failed to build $$t"; false; break; }; \
	done
	./scripts/feeds feed_config > tmp/.config-feeds.in.new
	cmp -s tmp/.config-feeds.in.new tmp/.config-feeds.in && rm -f tmp/.config-feeds.in.new || \
		mv tmp/.config-feeds.in.new tmp/.config-feeds.in
	$(call tmpinfo_gen,tmp/.packagedeps,mk)
	$(call tmpinfo_gen,tmp/.packageauxvars,pkgaux)
	$(call tmpinfo_gen,tmp/.packageusergroup,usergroup)
	touch $(TOPDIR)/tmp/.build

.config: ./scripts/config/conf $(if $(CONFIG_HAVE_DOT_CONFIG),,prepare-tmpinfo)
	@+if [ \! -e .config ] || ! grep CONFIG_HAVE_DOT_CONFIG .config >/dev/null; then \
		[ -e $(HOME)/.openwrt/defconfig ] && cp $(HOME)/.openwrt/defconfig .config; \
		$(_SINGLE)$(NO_TRACE_MAKE) menuconfig $(PREP_MK); \
	fi

ifeq ($(RECURSIVE_DEP_IS_ERROR),1)
  KCONF_FLAGS=--fatalrecursive
endif
ifneq ($(DISTRO_PKG_CONFIG),)
scripts/config/%onf: export PATH:=$(dir $(DISTRO_PKG_CONFIG)):$(PATH)
endif
scripts/config/%onf: CFLAGS+= -O2
scripts/config/%onf: FORCE
	@$(_SINGLE)$(SUBMAKE) $(if $(findstring s,$(OPENWRT_VERBOSE)),,-s) \
		-C scripts/config $(notdir $@)

$(eval $(call rdep,scripts/config,scripts/config/mconf))

config: scripts/config/conf prepare-tmpinfo FORCE
	[ -L .config ] && export KCONFIG_OVERWRITECONFIG=1; \
		$< $(KCONF_FLAGS) Config.in

config-clean: FORCE
	$(_SINGLE)$(NO_TRACE_MAKE) -C scripts/config clean

defconfig: scripts/config/conf prepare-tmpinfo FORCE
	touch .config
	@if [ ! -s .config -a -e $(HOME)/.openwrt/defconfig ]; then cp $(HOME)/.openwrt/defconfig .config; fi
	[ -L .config ] && export KCONFIG_OVERWRITECONFIG=1; \
		$< $(KCONF_FLAGS) --defconfig=.config Config.in

confdefault-y=allyes
confdefault-m=allmod
confdefault-n=allno
confdefault:=$(confdefault-$(CONFDEFAULT))

oldconfig: scripts/config/conf prepare-tmpinfo FORCE
	[ -L .config ] && export KCONFIG_OVERWRITECONFIG=1; \
		$< $(KCONF_FLAGS) --$(if $(confdefault),$(confdefault),old)config Config.in

menuconfig: scripts/config/mconf prepare-tmpinfo FORCE
	if [ \! -e .config -a -e $(HOME)/.openwrt/defconfig ]; then \
		cp $(HOME)/.openwrt/defconfig .config; \
	fi
	[ -L .config ] && export KCONFIG_OVERWRITECONFIG=1; \
		$< Config.in

nconfig: scripts/config/nconf prepare-tmpinfo FORCE
	if [ \! -e .config -a -e $(HOME)/.openwrt/defconfig ]; then \
		cp $(HOME)/.openwrt/defconfig .config; \
	fi
	[ -L .config ] && export KCONFIG_OVERWRITECONFIG=1; \
		$< Config.in

xconfig: scripts/config/qconf prepare-tmpinfo FORCE
	if [ \! -e .config -a -e $(HOME)/.openwrt/defconfig ]; then \
		cp $(HOME)/.openwrt/defconfig .config; \
	fi
	$< Config.in

prepare_kernel_conf: .config toolchain/install FORCE

ifeq ($(wildcard $(STAGING_DIR_HOST)/bin/quilt),)
  prepare_kernel_conf:
	@+$(SUBMAKE) -r tools/quilt/compile
else
  prepare_kernel_conf: ;
endif

kernel_oldconfig: prepare_kernel_conf
	$(_SINGLE)$(NO_TRACE_MAKE) -C target/linux oldconfig

ifneq ($(DISTRO_PKG_CONFIG),)
kernel_menuconfig: export PATH:=$(dir $(DISTRO_PKG_CONFIG)):$(PATH)
kernel_nconfig: export PATH:=$(dir $(DISTRO_PKG_CONFIG)):$(PATH)
kernel_xconfig: export PATH:=$(dir $(DISTRO_PKG_CONFIG)):$(PATH)
endif
kernel_menuconfig: prepare_kernel_conf
	$(_SINGLE)$(NO_TRACE_MAKE) -C target/linux menuconfig

kernel_nconfig: prepare_kernel_conf
	$(_SINGLE)$(NO_TRACE_MAKE) -C target/linux nconfig

kernel_xconfig: prepare_kernel_conf
	$(_SINGLE)$(NO_TRACE_MAKE) -C target/linux xconfig

$(STAGING_DIR_HOST)/.prereq-build: include/prereq-build.mk
	mkdir -p tmp
	@$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r -s -f $(TOPDIR)/include/prereq-build.mk prereq 2>/dev/null || { \
		echo "Prerequisite check failed. Use FORCE=1 to override."; \
		false; \
	}
  ifneq ($(realpath $(TOPDIR)/include/prepare.mk),)
	@$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r -s -f $(TOPDIR)/include/prepare.mk prepare 2>/dev/null || { \
		echo "Preparation failed."; \
		false; \
	}
  endif
	touch $@

printdb: FORCE
	@$(_SINGLE)$(NO_TRACE_MAKE) -p $@ V=99 DUMP_TARGET_DB=1 2>&1

ifndef SDK
  DOWNLOAD_DIRS = tools/download toolchain/download package/download target/download
else
  DOWNLOAD_DIRS = package/download
endif

download: .config FORCE $(if $(wildcard $(STAGING_DIR_HOST)/bin/flock),,tools/flock/compile) $(if $(wildcard $(STAGING_DIR_HOST)/bin/zstd),,tools/zstd/compile)
	@+$(foreach dir,$(DOWNLOAD_DIRS),$(SUBMAKE) $(dir);)

clean dirclean: .config
	@+$(SUBMAKE) -r $@

prereq:: prepare-tmpinfo .config
	@+$(NO_TRACE_MAKE) -r -s $@

check: .config FORCE
	@+$(NO_TRACE_MAKE) -r -s $@ QUIET= V=s

val.% var.%: FORCE
	@+$(NO_TRACE_MAKE) -r -s $@ QUIET= V=s

WARN_PARALLEL_ERROR = $(if $(BUILD_LOG),,$(and $(filter -j,$(MAKEFLAGS)),$(findstring s,$(OPENWRT_VERBOSE))))

ifeq ($(SDK),1)

%::
	@+$(PREP_MK) $(NO_TRACE_MAKE) -r -s prereq
	@./scripts/config/conf $(KCONF_FLAGS) --defconfig=.config Config.in
	@+$(ULIMIT_FIX) $(SUBMAKE) -r $@

else

# The check if .config is in sync takes about a second. Skip it if the last
# check passed, and neither .config nor a Kconfig file changed since then.
CONFIG_CHECK_STAMP:=tmp/.config-check
CONFIG_CHECK_FILES:=.config Config.in config target toolchain package feeds \
	tmp/.config-package.in tmp/.config-target.in tmp/.config-feeds.in scripts/config/conf

%::
	@+$(PREP_MK) $(NO_TRACE_MAKE) -r -s prereq
	@( \
		[ -f $(CONFIG_CHECK_STAMP) ] && [ -z "$$(find $(CONFIG_CHECK_FILES) -newer $(CONFIG_CHECK_STAMP) \
			\( -name .config -o -name conf -o -name '*.in' -o -name 'Config.*' \) -print -quit 2>/dev/null)" ] && exit 0; \
		touch $(CONFIG_CHECK_STAMP).new; \
		cp .config tmp/.config; \
		./scripts/config/conf $(KCONF_FLAGS) --defconfig=tmp/.config -w tmp/.config Config.in > /dev/null 2>&1; \
		if ./scripts/kconfig.pl '>' .config tmp/.config | grep -q CONFIG; then \
			rm -f $(CONFIG_CHECK_STAMP) $(CONFIG_CHECK_STAMP).new; \
			printf "$(_R)WARNING: your configuration is out of sync. Please run make menuconfig, oldconfig or defconfig!$(_N)\n" >&2; \
		else \
			mv $(CONFIG_CHECK_STAMP).new $(CONFIG_CHECK_STAMP); \
		fi \
	)
	@+$(ULIMIT_FIX) $(SUBMAKE) -r $@ $(if $(WARN_PARALLEL_ERROR), || { \
		printf "$(_R)Build failed - please re-run with -j1 to see the real error message$(_N)\n" >&2; \
		false; \
	} )

endif

# update all feeds, re-create index files, install symlinks
package/symlinks:
	./scripts/feeds update -a
	./scripts/feeds install -a

# re-create index files, install symlinks
package/symlinks-install:
	./scripts/feeds update -i
	./scripts/feeds install -a

# remove all symlinks, don't touch ./feeds
package/symlinks-clean:
	./scripts/feeds uninstall -a

help:
	cat README.md

distclean:
	rm -rf bin build_dir .ccache .config* dl feeds key-build* logs package/feeds target/linux/feeds staging_dir tmp
	@$(_SINGLE)$(SUBMAKE) -C scripts/config clean

ifeq ($(findstring v,$(DEBUG)),)
  .SILENT: symlinkclean clean dirclean distclean config-clean download help tmpinfo-clean .config scripts/config/mconf scripts/config/conf menuconfig $(STAGING_DIR_HOST)/.prereq-build tmp/.prereq-package prepare-tmpinfo
endif
.PHONY: help FORCE
.NOTPARALLEL:


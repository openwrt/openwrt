# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2007-2020 OpenWrt.org

PREP_MK= OPENWRT_BUILD= QUIET=

export IS_TTY=$(if $(MAKE_TERMOUT),1,0)

include $(TOPDIR)/include/verbose.mk

ifeq ($(SDK),1)
  include $(TOPDIR)/include/version.mk
else
  REVISION:=$(shell $(TOPDIR)/scripts/getver.sh)
  SOURCE_DATE_EPOCH:=$(shell $(TOPDIR)/scripts/get_source_date_epoch.sh)
endif

export REVISION
export SOURCE_DATE_EPOCH
export GIT_CONFIG_PARAMETERS='core.autocrlf=false'
export GIT_ASKPASS:=/bin/true
export MAKE_JOBSERVER=$(filter --jobserver%,$(MAKEFLAGS))
export GNU_HOST_NAME_GUESS:=$(shell $(TOPDIR)/scripts/config.guess)
export HOST_OS:=$(shell uname)

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
export PATH:=$(path)

unexport TAR_OPTIONS

ifeq ($(FORCE),)
  .config scripts/config/conf scripts/config/mconf: staging_dir/host/.prereq-build
endif

SCAN_COOKIE?=$(shell echo $$$$)
export SCAN_COOKIE

SUBMAKE:=umask 022; $(SUBMAKE)

ULIMIT_FIX=_limit=`ulimit -n`; [ "$$_limit" = "unlimited" -o "$$_limit" -ge 1024 ] || ulimit -n 1024;

ifdef SDK
  export IGNORE_PACKAGES = linux
  export DOWNLOAD_DIRS = package/download
else
  export DOWNLOAD_DIRS = tools/download toolchain/download target/download package/download
endif

prepare prepare-mk prepare-make prepare-build: config-clean prepare-clean tmpinfo-clean prereq-build prepare-tmpinfo FORCE ;

prereq-build: staging_dir/host/.prereq-build FORCE ;

ifneq ($(SDK),1)
prepare-clean: tools-clean
endif

prepare-clean: FORCE
	$(Q)rm -rf $(TOPDIR)/staging_dir/host/.prereq-build

tools-clean: FORCE
	$(Q)rm -rf $(TOPDIR)/staging_dir/host/.prereq-tools
	$(Q)$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r tools/flock/clean
	$(Q)$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r tools/mkhash/clean

tmpinfo-clean: FORCE
	$(Q)rm -rf $(TOPDIR)/tmp/.*info $(TOPDIR)/tmp/info

_ignore = $(foreach p,$(IGNORE_PACKAGES),--ignore $(p))

prepare-tmpinfo: FORCE
	$(Q)+$(MAKE) -r $(S) prereq-build $(PREP_MK)
	mkdir -p tmp/info
	$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r -f include/scan.mk SCAN_TARGET="packageinfo" SCAN_DIR="package" SCAN_NAME="package" SCAN_DEPTH=5 SCAN_EXTRA=""
	$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r -f include/scan.mk SCAN_TARGET="targetinfo" SCAN_DIR="target/linux" SCAN_NAME="target" SCAN_DEPTH=3 SCAN_EXTRA="" SCAN_MAKEOPTS="TARGET_BUILD=1"
	for type in package target; do \
		printf '%s' "Building $${type} config: "; \
		f=tmp/.$${type}info; t=tmp/.config-$${type}.in; \
		[ "$$t" -nt "$$f" ] || ./scripts/$${type}-metadata.pl $(_ignore) config "$$f" > "$$t" || { rm -f "$$t"; echo "Failed to build $$t"; false; break; }; \
		printf '%s\n' "done"; \
	done
	printf '%s\n' "Reading package info..."
	[ tmp/.config-feeds.in -nt tmp/.packageauxvars ] || ./scripts/feeds feed_config > tmp/.config-feeds.in
	./scripts/package-metadata.pl mk tmp/.packageinfo > tmp/.packagedeps || { rm -f tmp/.packagedeps; false; }
	./scripts/package-metadata.pl pkgaux tmp/.packageinfo > tmp/.packageauxvars || { rm -f tmp/.packageauxvars; false; }
	./scripts/package-metadata.pl usergroup tmp/.packageinfo > tmp/.packageusergroup || { rm -f tmp/.packageusergroup; false; }
	touch $(TOPDIR)/tmp/.build

.config: ./scripts/config/conf
	$(Q)+if [ \! -e .config ] || ! grep CONFIG_HAVE_DOT_CONFIG .config >/dev/null; then \
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
	$(Q)$(_SINGLE)$(SUBMAKE) $(S) -C scripts/config $(notdir $@)

$(eval $(call rdep,scripts/config,scripts/config/mconf))

config: scripts/config/conf $(if $(CONFIG_HAVE_DOT_CONFIG),,prepare-tmpinfo) FORCE
	[ -L .config ] && export KCONFIG_OVERWRITECONFIG=1; \
		$< $(KCONF_FLAGS) Config.in

config-clean: FORCE
	$(_SINGLE)$(NO_TRACE_MAKE) -C scripts/config clean

defconfig: scripts/config/conf $(if $(CONFIG_HAVE_DOT_CONFIG),,prepare-tmpinfo) FORCE
	touch .config
	$(Q)if [ ! -s .config -a -e $(HOME)/.openwrt/defconfig ]; then cp $(HOME)/.openwrt/defconfig .config; fi
	[ -L .config ] && export KCONFIG_OVERWRITECONFIG=1; \
		$< $(KCONF_FLAGS) --defconfig=.config Config.in

confdefault-y=allyes
confdefault-m=allmod
confdefault-n=allno
confdefault:=$(confdefault-$(CONFDEFAULT))

oldconfig: scripts/config/conf $(if $(CONFIG_HAVE_DOT_CONFIG),,prepare-tmpinfo) FORCE
	[ -L .config ] && export KCONFIG_OVERWRITECONFIG=1; \
		$< $(KCONF_FLAGS) --$(if $(confdefault),$(confdefault),old)config Config.in

menuconfig: scripts/config/mconf $(if $(CONFIG_HAVE_DOT_CONFIG),,prepare-tmpinfo) FORCE
	if [ \! -e .config -a -e $(HOME)/.openwrt/defconfig ]; then \
		cp $(HOME)/.openwrt/defconfig .config; \
	fi
	[ -L .config ] && export KCONFIG_OVERWRITECONFIG=1; \
		$< Config.in

nconfig: scripts/config/nconf $(if $(CONFIG_HAVE_DOT_CONFIG),,prepare-tmpinfo) FORCE
	if [ \! -e .config -a -e $(HOME)/.openwrt/defconfig ]; then \
		cp $(HOME)/.openwrt/defconfig .config; \
	fi
	[ -L .config ] && export KCONFIG_OVERWRITECONFIG=1; \
		$< Config.in

xconfig: scripts/config/qconf $(if $(CONFIG_HAVE_DOT_CONFIG),,prepare-tmpinfo) FORCE
	if [ \! -e .config -a -e $(HOME)/.openwrt/defconfig ]; then \
		cp $(HOME)/.openwrt/defconfig .config; \
	fi
	$< Config.in

prepare_kernel_conf: .config toolchain/install FORCE

ifeq ($(wildcard staging_dir/host/bin/quilt),)
  prepare_kernel_conf:
	$(Q)+$(SUBMAKE) $(S) -r tools/quilt/compile
else
  prepare_kernel_conf: ;
endif

kernel_oldconfig: prepare_kernel_conf FORCE
	$(_SINGLE)$(NO_TRACE_MAKE) -C target/linux oldconfig

ifneq ($(DISTRO_PKG_CONFIG),)
kernel_menuconfig: export PATH:=$(dir $(DISTRO_PKG_CONFIG)):$(PATH)
kernel_nconfig: export PATH:=$(dir $(DISTRO_PKG_CONFIG)):$(PATH)
kernel_xconfig: export PATH:=$(dir $(DISTRO_PKG_CONFIG)):$(PATH)
endif
kernel_menuconfig: prepare_kernel_conf FORCE
	$(_SINGLE)$(NO_TRACE_MAKE) -C target/linux menuconfig

kernel_nconfig: prepare_kernel_conf FORCE
	$(_SINGLE)$(NO_TRACE_MAKE) -C target/linux nconfig

kernel_xconfig: prepare_kernel_conf FORCE
	$(_SINGLE)$(NO_TRACE_MAKE) -C target/linux xconfig

ifneq ($(SDK),1)
staging_dir/host/.prereq-build: staging_dir/host/.prereq-tools
endif

staging_dir/host/.prereq-build: include/prereq-build.mk
	mkdir -p tmp
	$(Q)$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r -f $(TOPDIR)/include/prereq-build.mk prereq 2>/dev/null || { \
		echo "Prerequisite check failed. Use FORCE=1 to override."; \
		false; \
	}
  ifneq ($(realpath $(TOPDIR)/include/prepare.mk),)
	$(Q)$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r -f $(TOPDIR)/include/prepare.mk prepare 2>/dev/null || { \
		echo "Preparation failed."; \
		false; \
	}
  endif
	touch $@

staging_dir/host/.prereq-tools:
	$(Q)mkdir -p tmp
	$(Q)$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r tools/flock/compile
	$(Q)$(_SINGLE)$(NO_TRACE_MAKE) -j1 -r tools/mkhash/compile
	$(Q)touch $@

printdb: FORCE
	@$(_SINGLE)$(NO_TRACE_MAKE) -p $@ OPENWRT_VERBOSE=s DUMP_TARGET_DB=1 2>&1

clean dirclean: .config config-clean FORCE
	$(Q)+$(SUBMAKE) $(S) -r $@

download: prepare-tmpinfo .config FORCE
	$(Q)+$(foreach dir,$(DOWNLOAD_DIRS),$(SUBMAKE) $(S) $(dir);)

prereq:: prepare-tmpinfo .config FORCE
	$(Q)+$(NO_TRACE_MAKE) -r $@

check: .config FORCE
	@+$(NO_TRACE_MAKE) -r $@ OPENWRT_VERBOSE=s

val.%: FORCE
	@+$(NO_TRACE_MAKE) -r $@ OPENWRT_VERBOSE=s

var.%: FORCE
	@+$(NO_TRACE_MAKE) -r $@ OPENWRT_VERBOSE=s

type.%: FORCE
	@+$(NO_TRACE_MAKE) -r $@ OPENWRT_VERBOSE=s

host.%: FORCE
	@+$(NO_TRACE_MAKE) -r $@ OPENWRT_VERBOSE=s

WARN_PARALLEL_ERROR = $(if $(BUILD_LOG),,$(and $(filter -j,$(MAKEFLAGS)),$(findstring s,$(OPENWRT_VERBOSE))))

ifeq ($(SDK),1)

%::
	$(Q)+$(PREP_MK) $(NO_TRACE_MAKE) -r prereq
	$(Q)./scripts/config/conf $(KCONF_FLAGS) --defconfig=.config Config.in
	$(Q)+$(ULIMIT_FIX) $(SUBMAKE) $(S) -r $@

else

%::
	$(Q)+$(PREP_MK) $(NO_TRACE_MAKE) -r prereq
	$(Q)( \
		cp .config tmp/.config; \
		./scripts/config/conf $(KCONF_FLAGS) --defconfig=tmp/.config -w tmp/.config Config.in > /dev/null 2>&1; \
		if ./scripts/kconfig.pl '>' .config tmp/.config | grep -q CONFIG; then \
			printf "$(_R)WARNING: your configuration is out of sync. Please run make menuconfig, oldconfig or defconfig!$(_N)\n" >&2; \
		fi \
	)
	$(Q)+$(ULIMIT_FIX) $(SUBMAKE) $(S) -r $@ $(if $(WARN_PARALLEL_ERROR), || { \
		printf "$(_R)Build failed - please re-run with -j1 to see the real error message$(_N)\n" >&2; \
		false; \
	} )

endif

# update all feeds, re-create index files, install symlinks
package/symlinks update: FORCE
	./scripts/feeds update -a
	./scripts/feeds install -a

# re-create index files, install symlinks
package/symlinks-install symlinkinstall: FORCE
	./scripts/feeds update -i
	./scripts/feeds install -a

# remove all symlinks, don't touch ./feeds
package/symlinks-clean symlinkclean: FORCE
	./scripts/feeds uninstall -a

help: FORCE
	cat README.md

distclean: config-clean FORCE
	rm -rf bin build_dir .ccache .config* dl feeds key-build* logs package/feeds staging_dir tmp

ifeq ($(findstring v,$(DEBUG)),)
  .SILENT: symlinkclean clean dirclean distclean config-clean download help tmpinfo-clean prepare-clean .config scripts/config/mconf scripts/config/conf menuconfig staging_dir/host/.prereq-build prepare-tmpinfo
endif

.NOTPARALLEL:

FORCE: ;
.PHONY: FORCE

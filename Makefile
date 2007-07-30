# Makefile for OpenWrt
#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

TOPDIR:=${CURDIR}
LC_ALL:=C
LANG:=C
IS_TTY:=${shell tty -s && echo 1 || echo 0}
export TOPDIR LC_ALL LANG IS_TTY

world:

include $(TOPDIR)/include/debug.mk
include $(TOPDIR)/include/host.mk

ifneq ($(OPENWRT_BUILD),1)
  override OPENWRT_BUILD=1
  export OPENWRT_BUILD
  include $(TOPDIR)/include/toplevel.mk
else
  include rules.mk
  include $(INCLUDE_DIR)/depends.mk
  include $(INCLUDE_DIR)/subdir.mk
  include package/Makefile
  include tools/Makefile
  include toolchain/Makefile

clean: FORCE
	rm -rf build_* bin tmp

dirclean: clean
	rm -rf staging_dir_* toolchain_build_* tool_build

distclean: dirclean config-clean symlinkclean docs/clean
	rm -rf dl

target/%: FORCE
	$(MAKE) -C $(patsubst %/$*,%,$@) $*

# check prerequisites before starting to build
prereq: tmp/.prereq-target $(package/stamp-prereq) ;

world: .config $(tools/stamp-install) $(toolchain/stamp-install) FORCE
	$(MAKE) target/compile
	$(MAKE) package/compile
	$(MAKE) package/install
	$(MAKE) target/install
	$(MAKE) package/index

package/symlinks:
	$(SCRIPT_DIR)/feeds.sh $(CONFIG_SOURCE_FEEDS) $(CONFIG_SOURCE_FEEDS_REV)	

# FIXME: remove after converting target/ to new structure
tmp/.prereq-target: tmp/.targetinfo .config
tmp/.prereq-target: include/prereq.mk 
	mkdir -p tmp
	rm -f tmp/.host.mk
	@+$(NO_TRACE_MAKE) -s -C target prereq 2>/dev/null || { \
		echo "Prerequisite check failed. Use FORCE=1 to override."; \
		false; \
	}
	touch $@
.SILENT: tmp/.prereq-target

endif

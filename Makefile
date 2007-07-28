# Makefile for OpenWrt
#
# Copyright (C) 2006 OpenWrt.org
# Copyright (C) 2006 by Felix Fietkau <openwrt@nbd.name>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

all: world


TOPDIR:=${CURDIR}
LC_ALL:=C
LANG:=C
IS_TTY:=${shell tty -s && echo 1 || echo 0}
export TOPDIR LC_ALL LANG IS_TTY

include rules.mk

ifneq ($(OPENWRT_BUILD),1)
  export OPENWRT_BUILD:=1
  include $(INCLUDE_DIR)/toplevel.mk
else
  include $(INCLUDE_DIR)/depends.mk
  include $(INCLUDE_DIR)/subdir.mk
  include tools/Makefile

clean: FORCE
	rm -rf build_* bin tmp

dirclean: clean
	rm -rf staging_dir_* toolchain_build_* tool_build

distclean: dirclean config-clean symlinkclean docs/clean
	rm -rf dl

toolchain/% package/% target/%: FORCE
	$(MAKE) -C $(patsubst %/$*,%,$@) $*

world: .config $(tools/stamp) FORCE
	$(MAKE) toolchain/install
	$(MAKE) target/compile
	$(MAKE) package/compile
	$(MAKE) package/install
	$(MAKE) target/install
	$(MAKE) package/index

endif

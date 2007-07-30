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
  include $(INCLUDE_DIR)/target.mk
  include target/Makefile
  include package/Makefile
  include tools/Makefile
  include toolchain/Makefile

$(toolchain/stamp-compile): $(tools/stamp-compile)
$(target/stamp-compile): $(toolchain/stamp-install) $(tools/stamp-install)
$(package/stamp-compile): $(target/stamp-compile)
$(target/stamp-install): $(package/stamp-compile) $(package/stamp-install)

clean: FORCE
	rm -rf build_* bin tmp

dirclean: clean
	rm -rf staging_dir_* toolchain_build_* tool_build

distclean: dirclean 
	rm -rf dl .config*

# check prerequisites before starting to build
prereq: $(package/stamp-prereq) $(target/stamp-prereq) ;

world: .config $(tools/stamp-install) $(toolchain/stamp-install) $(target/stamp-compile) $(package/stamp-compile) $(package/stamp-install) $(target/stamp-install) FORCE
	$(MAKE) package/index

package/symlinks:
	$(SCRIPT_DIR)/feeds.sh $(CONFIG_SOURCE_FEEDS) $(CONFIG_SOURCE_FEEDS_REV)	

endif

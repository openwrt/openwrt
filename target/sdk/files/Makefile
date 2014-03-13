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
SDK:=1
export TOPDIR LC_ALL LANG SDK

world:

include $(TOPDIR)/include/host.mk

ifneq ($(OPENWRT_BUILD),1)
  override OPENWRT_BUILD=1
  export OPENWRT_BUILD

  empty:=
  space:= $(empty) $(empty)
  _SINGLE=export MAKEFLAGS=$(space);

  include $(TOPDIR)/include/debug.mk
  include $(TOPDIR)/include/depends.mk
  include $(TOPDIR)/include/toplevel.mk
else
  include rules.mk
  include $(INCLUDE_DIR)/depends.mk
  include $(INCLUDE_DIR)/subdir.mk
  include package/Makefile

$(package/stamp-compile): $(BUILD_DIR)/.prepared
$(BUILD_DIR)/.prepared: Makefile
	@mkdir -p $$(dirname $@)
	@mkdir -p bin/packages
	@touch $@

clean: FORCE
	rm -rf $(BUILD_DIR) $(BIN_DIR)

dirclean: clean
	rm -rf $(TMP_DIR)

# check prerequisites before starting to build
prereq: $(package/stamp-prereq) ;

world: prepare $(package/stamp-compile) FORCE
	@$(MAKE) package/index

.PHONY: clean dirclean prereq prepare world

endif

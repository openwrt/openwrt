# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/target.mk

clean download prepare compile install menuconfig update refresh: FORCE
	$(MAKE) -C $(BOARD)-$(KERNEL) $@


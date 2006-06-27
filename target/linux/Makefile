# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/target.mk

source:
	$(MAKE) -C $(BOARD)-$(KERNEL) source

prepare:
	rm -f $(TOPDIR)/.kernel.mk
	$(MAKE) -C $(BOARD)-$(KERNEL) prepare

compile:
	$(MAKE) -C $(BOARD)-$(KERNEL) compile

install:
	$(MAKE) -C $(BOARD)-$(KERNEL) install
	
clean:
	$(MAKE) -C $(BOARD)-$(KERNEL) clean

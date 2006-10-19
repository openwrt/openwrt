# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/target.mk

download:
	$(MAKE) -C $(BOARD)-$(KERNEL) download

prepare:
	$(MAKE) -C $(BOARD)-$(KERNEL) prepare

compile:
	$(MAKE) -C $(BOARD)-$(KERNEL) compile

install:
	$(MAKE) -C $(BOARD)-$(KERNEL) install
	
clean:
	$(MAKE) -C $(BOARD)-$(KERNEL) clean

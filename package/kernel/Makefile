# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kernel
PKG_VERSION:=$(LINUX_VERSION)-$(BOARD)
PKG_RELEASE:=$(LINUX_RELEASE)

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/packages

include $(INCLUDE_DIR)/package.mk
ifeq ($(DUMP),)
  -include $(LINUX_DIR)/.config
  NF_KMOD:=1
  include $(INCLUDE_DIR)/netfilter.mk
endif

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
endef

include $(TOPDIR)/target/linux/*/modules.mk
include ./modules/*.mk

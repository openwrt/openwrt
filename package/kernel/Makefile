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

# build all in-tree kmod-* packages by default
KMOD_DEFAULT:=m

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

CONFIG_PACKAGE_kernel=y
define Package/kernel
  SECTION:=sys
  CATEGORY:=Kernel
  DEFAULT:=y
  TITLE:=Virtual kernel package
  DESCRIPTION:=Virtual kernel package
  VERSION:=$(LINUX_VERSION)-$(BOARD)-$(LINUX_RELEASE)
endef

define Package/kernel/install
  # nothing to do
endef

# this is more robust than ifdef around eval/call
ifneq ($(DUMP),1)
  define BuildKernel
    $(call BuildPackage,kernel)
  endef
endif
$(eval $(call BuildKernel))

include $(TOPDIR)/target/linux/*/modules.mk
include ./modules/*.mk

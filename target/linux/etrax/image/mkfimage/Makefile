#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mkfimage
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Build/Compile
	mkdir -p $(PKG_BUILD_DIR)
	cp -r ./src/* $(PKG_BUILD_DIR)
	make -C $(PKG_BUILD_DIR)
endef

define Build/InstallDev
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mkfimage $(STAGING_DIR_HOST)/bin/mkfimage
endef

$(eval $(call Build/DefaultTargets))

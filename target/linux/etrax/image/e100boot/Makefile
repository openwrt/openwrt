#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=e100boot
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_SOURCE:=e100boot.tar.bz2
PKG_SOURCE_URL:=http://www.acmesystems.it/download/owrt \
http://download.tuxfamily.org/crisos/repository/source/
PKG_MD5SUM:=11fd53e7824dc550e4cffbc4cd114c3e

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

CRLF_WORKAROUND=1

include $(INCLUDE_DIR)/package.mk

define Build/Compile
	make -C $(PKG_BUILD_DIR) STRIP=true 
endef

define Build/InstallDev
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sbl/e100boot $(BIN_DIR)/etrax100boot
endef

$(eval $(call Build/DefaultTargets))

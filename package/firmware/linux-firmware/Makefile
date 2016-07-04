#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=linux-firmware
PKG_SOURCE_VERSION:=52442afee9907bc32a058f22bb3295d040677c26
PKG_VERSION:=2016-01-25-$(PKG_SOURCE_VERSION)
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)
PKG_SOURCE_URL:=git://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git
PKG_MIRROR_MD5SUM:=8d44332359de89b1936b4ff608a72614

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

SCAN_DEPS = *.mk

include $(INCLUDE_DIR)/package.mk

define Package/firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=http://git.kernel.org/cgit/linux/kernel/git/firmware/linux-firmware.git
  TITLE:=$(1)
endef

define Build/Compile

endef

include $(wildcard ./*.mk)
#$(eval $(call BuildPackage,linux-firmware))

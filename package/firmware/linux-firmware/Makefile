#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=linux-firmware
PKG_SOURCE_VERSION:=20250613
PKG_RELEASE:=2

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://gitlab.com/kernel-firmware/linux-firmware.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_MIRROR_HASH:=e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

SCAN_DEPS = *.mk

include $(INCLUDE_DIR)/package.mk

RSTRIP:=:
STRIP:=:

# optional build step override to add backport commits or revert commits
define Build/Prepare
	cd $(PKG_BUILD_DIR) && \
		git init && \
		git remote add origin $(PKG_SOURCE_URL) && \
		git fetch --depth=100 origin && \
		git checkout $(PKG_SOURCE_VERSION) && \
		git cherry-pick 9096bad65cb92da2925fe9ad891cbd4961fa6cda && \
		git cherry-pick a26e413e7481d12ab5a53f77e0cdde2d5be937d8 && \
		echo "Cherry-pick completed"
endef

define Package/firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://gitlab.com/kernel-firmware/linux-firmware
  TITLE:=$(1)
  DEPENDS:=$(2)
  LICENSE_FILES:=$(3)
  LICENSE:=$(4)
endef

define Build/Compile

endef

include $(wildcard ./*.mk)
#$(eval $(call BuildPackage,linux-firmware))

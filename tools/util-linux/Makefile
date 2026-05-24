#
# Copyright (C) 2006-2025 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=util-linux
PKG_VERSION:=2.42.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@KERNEL/linux/utils/$(PKG_NAME)/v2.42
PKG_HASH:=82e9158eb12a9b0b569d84e1687fed9dd18fe89ccd8ef5ac3427218a7c0d7f7f
PKG_CPE_ID:=cpe:/a:kernel:util-linux

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/meson.mk

MESON_HOST_ARGS += \
	$(if $(findstring y,$(YEAR_2038)),,-Dallow-32bit-time=true) \
	-Dauto_features=disabled \
	-Dbuild-hexdump=enabled \
	-Dbuild-libuuid=enabled \
	-Dncurses=enabled \
	-Dprogram-tests=false

$(eval $(call HostBuild))

#
# Copyright (C) 2006-2025 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=util-linux
PKG_VERSION:=2.42

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@KERNEL/linux/utils/$(PKG_NAME)/v2.42
PKG_HASH:=3452b260bbaa775d6e749ac3bb22111785003fc1f444970025c8da26dfa758e9
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

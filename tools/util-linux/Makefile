#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=util-linux
PKG_VERSION:=2.41.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@KERNEL/linux/utils/$(PKG_NAME)/v2.41
PKG_HASH:=be9ad9a276f4305ab7dd2f5225c8be1ff54352f565ff4dede9628c1aaa7dec57
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

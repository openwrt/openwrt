#
# Copyright (C) 2006-2025 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=util-linux
PKG_VERSION:=2.41.5

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@KERNEL/linux/utils/$(PKG_NAME)/v2.41
PKG_HASH:=f586e35d320ff537aab3ffeca37e9ecd482ccbe013590db4429a414d8aa6a728
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

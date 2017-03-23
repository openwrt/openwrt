# 
# Copyright (C) 2017 Yegor Yefremov <yegorslists@googlemail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=genimage
PKG_VERSION:=9

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=http://www.pengutronix.de/software/genimage/download
PKG_MIRROR_HASH:=69f35af7edf6f4dbdac7a18ddc69dcf38c501e43d8b50c524555754c51479078

HOST_BUILD_PARALLEL:=1
HOST_BUILD_DEPENDS:=confuse/host pkg-config/host

include $(INCLUDE_DIR)/host-build.mk

HOSTCC := $(HOSTCC_NOCACHE)
HOSTCXX := $(HOSTCXX_NOCACHE)

define Host/Install
	+$(MAKE) $(HOST_JOBS) -C $(HOST_BUILD_DIR) install
endef

$(eval $(call HostBuild))

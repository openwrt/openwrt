#
# Copyright (C) 2012-2015 OpenWrt.org
# Copyright (C) 2016 LEDE-Project.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=dosfstools
PKG_VERSION:=4.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://github.com/dosfstools/dosfstools/releases/download/v$(PKG_VERSION)/ \
		http://fossies.org/linux/misc
PKG_MD5SUM:=9037738953559d1efe04fc5408b6846216cc0138f7f9d32de80b6ec3c35e7daf

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

ifeq ($(HOST_OS),Darwin)
HOST_CFLAGS += -UHAVE_ENDIAN_H
LDLIBS += -liconv
endif

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/src/mkfs.fat $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mk{dosfs,fs.fat}
endef

$(eval $(call HostBuild))

# 
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=sdcc
PKG_VERSION:=3.0.0

PKG_SOURCE:=$(PKG_NAME)-src-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=20fbd49a3421e09fe65577c45524c89e

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)

include $(INCLUDE_DIR)/host-build.mk

HOST_LDFLAGS += $(HOST_STATIC_LINKING)
HOST_CONFIGURE_ARGS += \
	--disable-ucsim \
	--enable-packihx \
	--enable-sdcpp \
	--disable-sdcdb \
	--disable-gbz80-port \
	--disable-z80-port \
	--disable-ds390-port \
	--disable-ds400-port \
	--disable-pic-port \
	--disable-pic16-port \
	--disable-hc08-port \
	--disable-avr-port \
	--disable-xa51-port \
	--enable-mcs51-port \
	--enable-device-lib \

define Host/Clean
	-$(MAKE) -C $(HOST_BUILD_DIR) uninstall
	$(call Host/Clean/Default)
endef

$(eval $(call HostBuild))

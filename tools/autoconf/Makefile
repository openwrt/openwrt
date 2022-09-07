#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=autoconf
PKG_VERSION:=2.69

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@GNU/autoconf
PKG_HASH:=64ebcec9f8ac5b2487125a86a7760d2591ac9e1d3dbd59489633f9de62a57684

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_VARS += \
	PERL="/usr/bin/env perl"

define Host/Compile
	$(call Host/Compile/Default)
endef

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))

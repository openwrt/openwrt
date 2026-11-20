#
# Copyright (C) 2026 Michael Lotz <mmlr@mlotz.ch>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=bootflag
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/bootflag
	SECTION:=utils
	CATEGORY:=Base system
	TITLE:=Utility for reading and setting generic bootflags
	MAINTAINER:=Michael Lotz <mmlr@mlotz.ch>
endef

define Package/bootflag/description
	This package contains the bootflag utility to display and manipulate generic
	bootflags. It can be used to display and select the active boot image in
	dual-image setups for example.
endef

define Build/Compile
endef

define Package/bootflag/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/bootflag $(1)/usr/bin/bootflag
endef

$(eval $(call BuildPackage,bootflag))

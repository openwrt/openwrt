#
# Copyright (c) 2025 Pascal Beleiu <pascal@beleiu.de>
# Copyright (c) 2025 Eric Schäfer <eric@es86.de>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=zyxel-bootconfig-ipq807x
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/zyxel-bootconfig-ipq807x
	SECTION:=utils
	CATEGORY:=Base system
	TITLE:=Utility for handling Zyxel bootconfig settings on ipq807x devices
	MAINTAINER:=Pascal Beleiu <pascal@beleiu.de>
endef

define Package/zyxel-bootconfig-ipq807x/description
	This package contains an utility for handling Zyxel bootconfig settings on ipq807x devices.
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/zyxel-bootconfig-ipq807x/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/lib/preinit
	$(INSTALL_BIN) ./files/zyxel-bootconfig-ipq807x $(1)/usr/bin/
	$(CP) ./files/95_apply_bootconfig $(1)/lib/preinit/95_apply_bootconfig
endef

$(eval $(call BuildPackage,zyxel-bootconfig-ipq807x))

#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ifxmips_adsl
PKG_RELEASE:=1
PKG_VERSION:=1.0

include $(INCLUDE_DIR)/package.mk

define Package/ifxmips_adsl
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=@TARGET_ifxmips
  TITLE:=ifxmips adsl firmware loader
endef

define Package/ifxmips_adsl/description
 ifxmips adsl firmware loader
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(LINUX_DIR)/include/"
endef

define Package/ifxmips_adsl/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/{translate,danube_autoboot_daemon,danube_mei_debug,danube_show_firmware_date,cmvread,cmvwrite} $(1)/usr/bin/
endef

$(eval $(call BuildPackage,ifxmips_adsl))

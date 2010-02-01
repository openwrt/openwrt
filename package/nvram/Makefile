#
# Copyright (C) 2009 Jo-Philipp Wich <xm@subsignal.org>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nvram
PKG_RELEASE:=5

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/nvram
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=Userspace port of the Broadcom NVRAM manipulation tool
  DEPENDS:=@TARGET_brcm_2_4||@TARGET_brcm47xx||@TARGET_ar71xx
endef

define Package/nvram/description
 This package contains an utility to manipulate NVRAM on Broadcom based devices.
 It works on both brcm-2.4 and bcm47xx (Linux 2.6) without using the kernel api.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/nvram/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/nvram.init $(1)/etc/init.d/nvram
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/nvram $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libnvram.so.0.1 $(1)/usr/lib/
endef

$(eval $(call BuildPackage,nvram))

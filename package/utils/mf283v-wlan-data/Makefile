# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=mf283v-wlan-data
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=
PKG_MAINTAINER:=Dmitry Sinyavin <sinyavin@gmail.com>
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/mf283v-wlan-data
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=ZTE MF283V factory WLAN data extractor
  DEPENDS:=@TARGET_mdm9607 +ubi-utils
endef

define Package/mf283v-wlan-data/description
 Validate the redundant factory WLAN MAC and per-channel RF calibration data
 retained in the ZTE MF283V EFS and ZTEFILE partitions.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/mf283v-wlan-data.c $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) $(TARGET_LDFLAGS) \
		-Wall -Wextra -Werror -o $(PKG_BUILD_DIR)/mf283v-wlan-data \
		$(PKG_BUILD_DIR)/mf283v-wlan-data.c
endef

define Package/mf283v-wlan-data/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mf283v-wlan-data $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/mf283v-wlan.init $(1)/etc/init.d/mf283v-wlan
endef

$(eval $(call BuildPackage,mf283v-wlan-data))

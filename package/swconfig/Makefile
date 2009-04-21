# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-swconfig
PKG_RELEASE:=1
PKG_BUILD_DEPENDS:=libnl

include $(INCLUDE_DIR)/package.mk

define Package/swconfig
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=@LINUX_2_6_26||LINUX_2_6_27||LINUX_2_6_28||LINUX_2_6_29 +libuci
  TITLE:=Switch configuration utility
endef

TARGET_CPPFLAGS += \
	-I$(LINUX_DIR)/include \
	-I$(PKG_BUILD_DIR)

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		LIBS="-L$(STAGING_DIR)/usr/lib $(STAGING_DIR)/usr/lib/libnl.a -lm -luci"
endef

define Package/swconfig/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/swconfig $(1)/bin/swconfig
endef

$(eval $(call BuildPackage,swconfig))

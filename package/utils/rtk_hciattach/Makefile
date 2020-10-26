# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=rtk_hciattach
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/seuscq/rtk_hciattach.git
PKG_MIRROR_HASH:=12a98464ed3e96b89b7df7cb267db5ad1ea87229591f2a8d8ff10df0965ab950
PKG_SOURCE_DATE:=2018-07-09
PKG_SOURCE_VERSION:=8dba68d3456106a995a9ee1c0b6fdfaa61965a42
PKG_MAINTAINER:=Ivan Belokobylskiy <belokobylskij@gmail.com>
PKG_LICENSE:=GPL-2.0-or-later

CC = gcc
CFLAGS = -Wall
LDFLAGS =

include $(INCLUDE_DIR)/package.mk


define Package/rtk_hciattach
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=rtk_hciattach bluetooth setup for rtl8723bs_bt
endef

define Package/rtk_hciattach/description
	A tool to setup bluetooth for rtl8723bs bluetooth
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/hciattach_rtk.o -c $(PKG_BUILD_DIR)/hciattach_rtk.c
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/hciattach.o -c $(PKG_BUILD_DIR)/hciattach.c
	$(TARGET_CC) $(TARGET_LDFLAGS) -o $(PKG_BUILD_DIR)/$1 $(PKG_BUILD_DIR)/hciattach_rtk.o $(PKG_BUILD_DIR)/hciattach.o
endef

define Package/rtk_hciattach/conffiles
/etc/config/rtk_hciattach
endef

define Package/rtk_hciattach/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc/init.d $(1)/etc/config $(1)/lib/firmware/rtl_bt/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rtk_hciattach $(1)/usr/bin/
	$(CP) ./files/rtlbt_fw $(1)/lib/firmware/rtl_bt/
	$(INSTALL_BIN) ./files/rtk_hciattach.init $(1)/etc/init.d/rtk_hciattach
	$(INSTALL_CONF) ./files/rtk_hciattach.config $(1)/etc/config/rtk_hciattach
endef

$(eval $(call BuildPackage,rtk_hciattach))

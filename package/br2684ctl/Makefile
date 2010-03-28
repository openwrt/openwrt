# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=br2684ctl
PKG_VERSION:=20040226
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/b/br2684ctl
PKG_MD5SUM:=6eb4d8cd174e24a7c078eb4f594f5b69

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION).orig
PKG_BUILD_DEPENDS:=linux-atm

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=net
  CATEGORY:=Base system
  TITLE:=ATM Ethernet bridging configuration utility
  DEPENDS:=+linux-atm
  URL:=http://ftp.debian.org/debian/pool/main/b/br2684ctl
endef

TARGET_CPPFLAGS += -I$(LINUX_DIR)/include
MAKE_FLAGS += CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS) $(TARGET_LDFLAGS)"

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/br2684ctl $(1)/etc/init.d/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

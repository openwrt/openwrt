#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ifenslave
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/ifenslave
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+kmod-bonding @(LINUX_3_3||LINUX_3_6||LINUX_3_7||LINUX_3_8||LINUX_3_9||LINUX_3_10)
  TITLE:=A utility to manipulate kernel ethernet bonding devices
  VERSION:=$(LINUX_VERSION)-$(PKG_RELEASE)
  URL:=http://www.kernel.org
endef

define Package/ifenslave/description
 ifenslave is needed to add/remove interfaces to/from ethernet bond devices.
endef

define Build/Prepare
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/ifenslave \
		$(LINUX_DIR)/Documentation/networking/ifenslave.c
endef

define Package/ifenslave/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ifenslave $(1)/sbin/
endef

$(eval $(call BuildPackage,ifenslave))

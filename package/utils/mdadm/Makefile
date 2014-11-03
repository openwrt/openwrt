#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mdadm
PKG_VERSION:=3.2.5
PKG_RELEASE:=1

PKG_SOURCE_URL:=@KERNEL/linux/utils/raid/mdadm
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_MD5SUM:=2fd33dedcdb06f0d1461f50ddabb7e4a

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

PKG_LICENSE:=GPL-2.0+
PKG_LICENSE_FILES:=COPYING

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/mdadm
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=A tool for managing Soft RAID under Linux
  URL:=http://www.kernel.org/pub/linux/utils/raid/mdadm/
  DEPENDS:=+@KERNEL_DIRECT_IO
endef

define Package/mdadm/description
 A tool for managing Linux Software RAID arrays.
endef

TARGET_CFLAGS += -ffunction-sections -fdata-sections
TARGET_LDFLAGS += -Wl,--gc-sections

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -DHAVE_STDINT_H" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		mdadm
endef

define Package/mdadm/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mdadm $(1)/sbin
endef

$(eval $(call BuildPackage,mdadm))

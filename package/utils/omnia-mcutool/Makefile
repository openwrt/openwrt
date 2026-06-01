#
# Copyright (C) 2016-2024 CZ.NIC z.s.p.o. (http://www.nic.cz/)
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=omnia-mcutool
PKG_VERSION_REAL:=0.3-rc4
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://gitlab.nic.cz/turris/$(PKG_NAME)
PKG_SOURCE_DATE:=2026-06-01
PKG_SOURCE_VERSION:=195391be28d243e047ac0b1464c79c8cf0ee3125
PKG_MIRROR_HASH:=488aacca75f0a9bc0020098f13579bcf55179e91f6a199836c079152e133678a

PKG_MAINTAINER:=Marek Mojik <marek.mojik@nic.cz>
PKG_LICENSE:=GPL-2.0-or-later

include $(INCLUDE_DIR)/package.mk

define Package/omnia-mcutool
  SECTION:=utils
  CATEGORY:=Utilities
  URL:=https://gitlab.nic.cz/turris/$(PKG_NAME)
  TITLE:=CZ.NIC Turris Omnia MCU utility
  DEPENDS:=+libopenssl +omnia-mcu-firmware
endef

define Package/omnia-mcutool/description
The omnia-mcutool utility is mainly used to upgrade the firmware on the
microcontroller on the Turris Omnia router. It can also show state of MCU
settings and configure MCU options (GPIOs, LEDs, power).
endef

TARGET_LDFLAGS += -lcrypto

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		MCUTOOL_VERSION="$(PKG_VERSION_REAL)"
endef

define Package/omnia-mcutool/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/omnia-mcutool $(1)/usr/bin/
endef

$(eval $(call BuildPackage,omnia-mcutool))

#
# Copyright (C) 2024 Marek Behún <kabel@kernel.org>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=omnia-eeprom
PKG_VERSION:=v0.1
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://gitlab.nic.cz/turris/$(PKG_NAME)/-/archive/$(PKG_VERSION)/
PKG_HASH:=6f949d0b8080adca8bae088774ce615b563ba6ec2807cce97ee6769b4eee7bbf

PKG_MAINTAINER:=Marek Behun <kabel@kernel.org>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/omnia-eeprom
  SECTION:=utils
  CATEGORY:=Utilities
  URL:=https://gitlab.nic.cz/turris/$(PKG_NAME)
  TITLE:=CZ.NIC Turris Omnia EEPROM accessing utility
  DEPENDS:=@TARGET_mvebu_cortexa9_DEVICE_cznic_turris-omnia
endef

define Package/omnia-eeprom/description
This package contains the omnia-eeprom utility, which allows you to display
and update EEPROM fields on the Turris Omnia router.
The EEPROM is normally not meant to be updated by users, but there are some
exceptions where it might be useful.
One such example is to change the DDR3 speed from the default 1600K mode to
1333H mode, in order to solve random crashes that occur on some boards with
newer versions of the U-Boot bootloader (because of bugs in newer versions of
the DDR training algorithm).
endef

MAKE_VARS += \
	OMNIA_EEPROM_VERSION="$(PKG_VERSION)"

TARGET_CFLAGS += -Wall

Build/Compile = $(Build/Compile/Default)

define Package/omnia-eeprom/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/omnia-eeprom $(1)/usr/bin/
endef

$(eval $(call BuildPackage,omnia-eeprom))

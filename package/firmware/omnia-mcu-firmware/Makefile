#
# Copyright (C) 2024 CZ.NIC z.s.p.o. (http://www.nic.cz/)
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=omnia-mcu-firmware
PKG_DISTNAME:=omnia_hw_ctrl
PKG_VERSION:=4.0
PKG_RELEASE:=1

PKG_SOURCE_SUBDIR:=$(PKG_DISTNAME)-binaries-v$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.bz2
PKG_SOURCE_URL:=https://gitlab.nic.cz/turris/hw/$(PKG_DISTNAME)/-/releases/v$(PKG_VERSION)/downloads/
PKG_HASH:=64b6010f587d89c192685d152644075a5e0abf431eb069ac42981524065a0053

PKG_MAINTAINER:=Marek Mojik <marek.mojik@nic.cz>
PKG_LICENSE:=GPL-3.0-or-later

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk

define Package/omnia-mcu-firmware
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://gitlab.nic.cz/turris/hw/$(PKG_DISTNAME)/-/releases
  TITLE:=CZ.NIC Turris Omnia MCU firmware
  DEPENDS:=@TARGET_mvebu_cortexa9_DEVICE_cznic_turris-omnia
endef

define Package/omnia-mcu-firmware/description
Firmware binaries for the microcontroller on the Turris Omnia router. These are
used by the omnia-mcutool utility when upgrading MCU firmware.
endef

define Build/Compile
	true
endef

define Package/omnia-mcu-firmware/install
	$(INSTALL_DIR) $(1)/usr/share/omnia-mcu-firmware/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/*.bin $(1)/usr/share/omnia-mcu-firmware/
endef

$(eval $(call BuildPackage,omnia-mcu-firmware))

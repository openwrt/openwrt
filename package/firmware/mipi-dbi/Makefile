# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=mipi-dbi
PKG_RELEASE:=1

PKG_MAINTAINER:=Matt Eaton <linux@divinehawk.com>

include $(INCLUDE_DIR)/package.mk

define Package/mipi-dbi-default
  SECTION:=firmware
  CATEGORY:=Firmware
endef

define Build/Compile
	true
endef

define Package/mipi-dbi-ubnt-utr
  $(Package/mipi-dbi-default)
  TITLE:=MIPI DBI LCD init sequence for Ubiquiti UniFi Travel Router
endef

define Package/mipi-dbi-ubnt-utr/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) ./files/ubnt,utr-lcd.bin $(1)/lib/firmware/
endef

$(eval $(call BuildPackage,mipi-dbi-ubnt-utr))

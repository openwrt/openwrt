# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=mipi-dbi
PKG_RELEASE:=3

PKG_LICENSE:=GPL-2.0-or-later
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=Matt Eaton <linux@divinehawk.com>

include $(INCLUDE_DIR)/package.mk

# The panel-mipi-dbi driver requests "<compatible[0]>.bin", so each init
# sequence is named after the compatible string of the panel it drives.
MIPI_DBI_SEQUENCES:=$(notdir $(wildcard ./files/*.txt))

define Package/mipi-dbi-default
  SECTION:=firmware
  CATEGORY:=Firmware
  PKGARCH:=all
endef

define Build/Compile
	$(foreach seq,$(MIPI_DBI_SEQUENCES), \
		$(STAGING_DIR_HOST)/bin/python3 ./mipi-dbi-cmd \
			$(PKG_BUILD_DIR)/$(seq:.txt=.bin) ./files/$(seq) || exit 1;)
endef

define Package/mipi-dbi-ubnt-utr
  $(Package/mipi-dbi-default)
  TITLE:=MIPI DBI LCD init sequence for Ubiquiti UniFi Travel Router
  DEPENDS:=@TARGET_ipq40xx
endef

define Package/mipi-dbi-ubnt-utr/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/ubnt,utr-lcd.bin $(1)/lib/firmware/
endef

$(eval $(call BuildPackage,mipi-dbi-ubnt-utr))

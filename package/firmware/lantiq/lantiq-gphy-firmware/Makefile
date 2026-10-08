# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=lantiq-gphy-firmware
PKG_RELEASE:=1
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/lantiq-gphy-firmware-defaults
  SECTION:=firmware
  CATEGORY:=Firmware
  DEPENDS:=@TARGET_lantiq_xrx200||TARGET_lantiq_xrx200_legacy
  DEFAULT:=n
  TITLE:=$(1)
endef

define Package/lantiq-gphy-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/lantiq
	$(INSTALL_DATA) files/$(2) $(1)/lib/firmware/lantiq/$(3)
endef

define Package/xrx200-rev1.1-phy11g-firmware
  $(call Package/lantiq-gphy-firmware-defaults, xRx200 rev 1.1 Gigabit Ethernet PHY Firmware)
endef

define Package/xrx200-rev1.1-phy11g-firmware/description
  This package contains firmware for Gigabit Ethernet PHY integrated in xRx200 rev 1.1 SoC.
endef

define Package/xrx200-rev1.1-phy11g-firmware/install
	$(call Package/lantiq-gphy-firmware/install,$(1),xrx200_phy11g_a14.bin,xrx200_phy11g_a14.bin)
endef

define Package/xrx200-rev1.2-phy11g-firmware
  $(call Package/lantiq-gphy-firmware-defaults, xRx200 rev 1.2 Gigabit Ethernet PHY Firmware)
endef

define Package/xrx200-rev1.2-phy11g-firmware/description
  This package contains firmware for Gigabit Ethernet PHY integrated in xRx200 rev 1.2 SoC.
endef

define Package/xrx200-rev1.2-phy11g-firmware/install
	$(call Package/lantiq-gphy-firmware/install,$(1),xrx200_phy11g_a22.bin,xrx200_phy11g_a22.bin)
endef

define Package/xrx200-rev1.1-phy22f-firmware
  $(call Package/lantiq-gphy-firmware-defaults, xRx200 rev 1.1 Fast Ethernet PHY Firmware)
endef

define Package/xrx200-rev1.1-phy22f-firmware/description
  This package contains firmware for Fast Ethernet PHY integrated in xRx200 rev 1.1 SoC.
endef

define Package/xrx200-rev1.1-phy22f-firmware/install
	$(call Package/lantiq-gphy-firmware/install,$(1),xrx200_phy22f_a14.bin,xrx200_phy22f_a14.bin)
endef

define Package/xrx200-rev1.2-phy22f-firmware
  $(call Package/lantiq-gphy-firmware-defaults, xRx200 rev 1.2 Fast Ethernet PHY Firmware)
endef

define Package/xrx200-rev1.2-phy22f-firmware/description
  This package contains firmware for Fast Ethernet PHY integrated in xRx200 rev 1.2 SoC.
endef

define Package/xrx200-rev1.2-phy22f-firmware/install
	$(call Package/lantiq-gphy-firmware/install,$(1),xrx200_phy22f_a22.bin,xrx200_phy22f_a22.bin)
endef

define Build/Compile
endef

$(eval $(call BuildPackage,xrx200-rev1.1-phy11g-firmware))
$(eval $(call BuildPackage,xrx200-rev1.2-phy11g-firmware))
$(eval $(call BuildPackage,xrx200-rev1.1-phy22f-firmware))
$(eval $(call BuildPackage,xrx200-rev1.2-phy22f-firmware))

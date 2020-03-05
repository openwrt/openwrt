#
# Copyright (C) 2017-2019 Chen Minqiang <ptpt52@gmail.com>
# Copyright (C) 2019 X-WRT <dev@x-wrt.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=k3-custom-firmware
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_MAINTAINER:=X-WRT <dev@x-wrt.com>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/k3-custom-firmware/Default
  SECTION:=Applications
  CATEGORY:=Independent
  TITLE:=Custom Firmware for K3
  DEPENDS:=@(TARGET_bcm53xx_generic_DEVICE_phicomm_k3||TARGET_DEVICE_bcm53xx_generic_DEVICE_phicomm_k3)
endef

define Package/k3-custom-firmware
  $(call Package/k3-custom-firmware/Default)
endef

define Package/k3-custom-firmware/description
  Standalone packages for K3
endef

define Package/k3-custom-firmware/install-overlay
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) ./files/brcmfmac4366c-pcie.bin $(1)/lib/firmware/brcm/
endef

define Build/Configure
  true
endef

define Build/Prepare
  true
endef

define Build/Compile
  true
endef

$(eval $(call BuildPackage,k3-custom-firmware))

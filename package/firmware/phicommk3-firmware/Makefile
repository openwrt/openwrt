include $(TOPDIR)/rules.mk

PKG_NAME:=phicommk3-firmware
PKG_VERSION:=2
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/Hill-98/phicommk3-firmware.git
PKG_SOURCE_VERSION:=b07f42fdb61d286bab29bf5b097fb0937e379187
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk

define Build/Configure
endef

define Build/Compile
endef

define Package/$(PKG_NAME)
	SECTION:=firmware
	CATEGORY:=Firmware
	TITLE:=PHICOMM K3 available wireless firmware
	DEPENDS:=@TARGET_bcm53xx_DEVICE_phicomm-k3
endef

define Package/$(PKG_NAME)/config
	source "$(SOURCE)/Config.in"
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(if $(CONFIG_K3_WF_AC88U),$(INSTALL_DATA) $(PKG_BUILD_DIR)/brcmfmac4366c-pcie.bin.ac88u $(1)/lib/firmware/brcm/brcmfmac4366c-pcie.bin)
	$(if $(CONFIG_K3_WF_EA9500),$(INSTALL_DATA) $(PKG_BUILD_DIR)/brcmfmac4366c-pcie.bin.ea9500 $(1)/lib/firmware/brcm/brcmfmac4366c-pcie.bin)
	$(if $(CONFIG_K3_WF_K3),$(INSTALL_DATA) $(PKG_BUILD_DIR)/brcmfmac4366c-pcie.bin.k3 $(1)/lib/firmware/brcm/brcmfmac4366c-pcie.bin)
	$(if $(CONFIG_K3_WF_DHD24),$(INSTALL_DATA) $(PKG_BUILD_DIR)/brcmfmac4366c-pcie.bin.asus-dhd24 $(1)/lib/firmware/brcm/brcmfmac4366c-pcie.bin)
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

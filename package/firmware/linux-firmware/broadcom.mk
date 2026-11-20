define Package/brcmfmac-firmware-43340-sdio
  $(call Package/firmware-default,Broadcom BCM43340 FullMac SDIO firmware,,LICENSES/LICENCE.cypress)
  PROVIDES:=cypress-firmware-43340-sdio
  CONFLICTS:=cypress-firmware-43340-sdio
endef

define Package/brcmfmac-firmware-43340-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cypress/cyfmac43340-sdio.bin \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac43340-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac43340-sdio.bin
endef
$(eval $(call BuildPackage,brcmfmac-firmware-43340-sdio))

define Package/brcmfmac-firmware-43362-sdio
  $(call Package/firmware-default,Broadcom BCM43362 FullMac SDIO firmware,,LICENSES/LICENCE.cypress)
  PROVIDES:=cypress-firmware-43362-sdio
  CONFLICTS:=cypress-firmware-43362-sdio
endef

define Package/brcmfmac-firmware-43362-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cypress/cyfmac43362-sdio.bin \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac43362-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac43362-sdio.bin
endef
$(eval $(call BuildPackage,brcmfmac-firmware-43362-sdio))

define Package/brcmfmac-firmware-4354-sdio
  $(call Package/firmware-default,Broadcom BCM4354 FullMac SDIO firmware,,LICENSES/LICENCE.cypress)
  PROVIDES:=cypress-firmware-4354-sdio
  CONFLICTS:=cypress-firmware-4354-sdio
endef

define Package/brcmfmac-firmware-4354-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cypress/cyfmac4354-sdio.bin \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cypress/cyfmac4354-sdio.clm_blob \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac4354-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac4354-sdio.bin
	$(LN) \
		../cypress/cyfmac4354-sdio.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac4354-sdio.clm_blob
endef
$(eval $(call BuildPackage,brcmfmac-firmware-4354-sdio))

define Package/brcmfmac-firmware-4356-pcie
  $(call Package/firmware-default,Broadcom BCM4356 FullMac PCIe firmware,,LICENSES/LICENCE.cypress)
  PROVIDES:=cypress-firmware-4356-pcie
  CONFLICTS:=cypress-firmware-4356-pcie
endef

define Package/brcmfmac-firmware-4356-pcie/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cypress/cyfmac4356-pcie.bin \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cypress/cyfmac4356-pcie.clm_blob \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac4356-pcie.bin \
		$(1)/lib/firmware/brcm/brcmfmac4356-pcie.bin
	$(LN) \
		../cypress/cyfmac4356-pcie.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac4356-pcie.clm_blob
endef
$(eval $(call BuildPackage,brcmfmac-firmware-4356-pcie))

define Package/brcmfmac-firmware-4356-sdio
  $(call Package/firmware-default,Broadcom BCM4356 FullMac SDIO firmware,,LICENSES/LICENCE.cypress)
  PROVIDES:=cypress-firmware-4356-sdio
  CONFLICTS:=cypress-firmware-4356-sdio
endef

define Package/brcmfmac-firmware-4356-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cypress/cyfmac4356-sdio.bin \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cypress/cyfmac4356-sdio.clm_blob \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac4356-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac4356-sdio.bin
	$(LN) \
		../cypress/cyfmac4356-sdio.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac4356-sdio.clm_blob
endef
$(eval $(call BuildPackage,brcmfmac-firmware-4356-sdio))

define Package/brcmfmac-firmware-43570-pcie
  $(call Package/firmware-default,Broadcom BCM43570 FullMac PCIe firmware,,LICENSES/LICENCE.cypress)
  PROVIDES:=cypress-firmware-43570-pcie
  CONFLICTS:=cypress-firmware-43570-pcie
endef

define Package/brcmfmac-firmware-43570-pcie/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cypress/cyfmac43570-pcie.bin \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cypress/cyfmac43570-pcie.clm_blob \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac43570-pcie.bin \
		$(1)/lib/firmware/brcm/brcmfmac43570-pcie.bin
	$(LN) \
		../cypress/cyfmac43570-pcie.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac43570-pcie.clm_blob
endef
$(eval $(call BuildPackage,brcmfmac-firmware-43570-pcie))

define Package/brcmfmac-firmware-4339-sdio
  $(call Package/firmware-default,Broadcom 4339 FullMAC SDIO firmware,,LICENSES/LICENCE.cypress)
  PROVIDES:=cypress-firmware-4339-sdio
  CONFLICTS:=cypress-firmware-4339-sdio
endef
define Package/brcmfmac-firmware-4339-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/cypress
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cypress/cyfmac4339-sdio.bin \
		$(1)/lib/firmware/cypress/
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(LN) \
		../cypress/cyfmac4339-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac4339-sdio.bin
endef
$(eval $(call BuildPackage,brcmfmac-firmware-4339-sdio))

Package/brcmfmac-firmware-43602a1-pcie = $(call Package/firmware-default,Broadcom 43602a1 FullMAC PCIe firmware,,LICENCE.broadcom_bcm43xx)
define Package/brcmfmac-firmware-43602a1-pcie/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43602-pcie.ap.bin \
		$(1)/lib/firmware/brcm/brcmfmac43602-pcie.bin
endef
$(eval $(call BuildPackage,brcmfmac-firmware-43602a1-pcie))

Package/brcmfmac-firmware-4366b1-pcie = $(call Package/firmware-default,Broadcom 4366b1 FullMAC PCIe firmware,,LICENCE.broadcom_bcm43xx)
define Package/brcmfmac-firmware-4366b1-pcie/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac4366b-pcie.bin \
		$(1)/lib/firmware/brcm/
endef
$(eval $(call BuildPackage,brcmfmac-firmware-4366b1-pcie))

Package/brcmfmac-firmware-4366c0-pcie = $(call Package/firmware-default,Broadcom 4366c0 FullMAC PCIe firmware,,LICENCE.broadcom_bcm43xx)
define Package/brcmfmac-firmware-4366c0-pcie/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac4366c-pcie.bin \
		$(1)/lib/firmware/brcm/
endef
$(eval $(call BuildPackage,brcmfmac-firmware-4366c0-pcie))

Package/brcmfmac-firmware-4329-sdio = $(call Package/firmware-default,Broadcom BCM4329 FullMac SDIO firmware,,LICENCE.broadcom_bcm43xx)
define Package/brcmfmac-firmware-4329-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac4329-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac4329-sdio.bin
endef
$(eval $(call BuildPackage,brcmfmac-firmware-4329-sdio))

Package/brcmfmac-nvram-43430-sdio = $(call Package/firmware-default,Broadcom BCM43430 SDIO NVRAM,,LICENCE.broadcom_bcm43xx)
define Package/brcmfmac-nvram-43430-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43430-sdio.AP6212.txt \
		$(1)/lib/firmware/brcm/
	$(LN) \
		brcmfmac43430-sdio.AP6212.txt \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.sinovoip,bpi-m2-plus.txt
	$(LN) \
		brcmfmac43430-sdio.AP6212.txt \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.sinovoip,bpi-m2-zero.txt
	$(LN) \
		brcmfmac43430-sdio.AP6212.txt \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.sinovoip,bpi-m2-ultra.txt
	$(LN) \
		brcmfmac43430-sdio.AP6212.txt \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.sinovoip,bpi-m3.txt
	$(LN) \
		brcmfmac43430-sdio.AP6212.txt \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.friendlyarm,nanopi-r1.txt
	$(LN) \
		brcmfmac43430-sdio.AP6212.txt \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.starfive,visionfive-v1.txt
	$(LN) \
		brcmfmac43430-sdio.AP6212.txt \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.beagle,beaglev-starlight-jh7100-a1.txt
	$(LN) \
		brcmfmac43430-sdio.AP6212.txt \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.beagle,beaglev-starlight-jh7100-r0.txt
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43430-sdio.Hampoo-D2D3_Vi8A1.txt \
		$(1)/lib/firmware/brcm/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43430-sdio.MUR1DX.txt \
		$(1)/lib/firmware/brcm/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43430-sdio.raspberrypi,3-model-b.txt \
		$(1)/lib/firmware/brcm/
	$(LN) \
		brcmfmac43430-sdio.raspberrypi,3-model-b.txt \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.raspberrypi,model-zero-w.txt
	$(LN) \
		brcmfmac43430-sdio.raspberrypi,3-model-b.txt \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.raspberrypi,model-zero-2-w.txt
endef
$(eval $(call BuildPackage,brcmfmac-nvram-43430-sdio))

Package/brcmfmac-firmware-43430a0-sdio = $(call Package/firmware-default,Broadcom BCM43430a0 FullMac SDIO firmware,,LICENCE.broadcom_bcm43xx)
define Package/brcmfmac-firmware-43430a0-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43430a0-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac43430a0-sdio.bin
endef
$(eval $(call BuildPackage,brcmfmac-firmware-43430a0-sdio))

Package/brcmfmac-nvram-43455-sdio = $(call Package/firmware-default,Broadcom BCM43455 SDIO NVRAM,,LICENCE.broadcom_bcm43xx)
define Package/brcmfmac-nvram-43455-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43455-sdio.acepc-t8.txt \
		$(1)/lib/firmware/brcm/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43455-sdio.raspberrypi,3-model-b-plus.txt \
		$(1)/lib/firmware/brcm/
	$(LN) \
		brcmfmac43455-sdio.raspberrypi,3-model-b-plus.txt \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.raspberrypi,3-model-a-plus.txt
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43455-sdio.raspberrypi,4-model-b.txt \
		$(1)/lib/firmware/brcm/
	$(LN) \
		brcmfmac43455-sdio.raspberrypi,4-model-b.txt \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.raspberrypi,4-compute-module.txt
	$(LN) \
		brcmfmac43455-sdio.raspberrypi,4-model-b.txt \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.Raspberry\ Pi\ Foundation-Raspberry\ Pi\ 4\ Model\ B.txt
	$(LN) \
		brcmfmac43455-sdio.raspberrypi,4-model-b.txt \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.raspberrypi,5-model-b.txt
	$(LN) \
		brcmfmac43455-sdio.raspberrypi,4-model-b.txt \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.raspberrypi,5-compute-module.txt
	$(LN) \
		brcmfmac43455-sdio.raspberrypi,4-model-b.txt \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.Raspberry\ Pi\ Foundation-Raspberry\ Pi\ Compute\ Module\ 4.txt
	$(LN) \
		brcmfmac43455-sdio.raspberrypi,4-model-b.txt \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.Raspberry\ Pi\ Foundation-Raspberry\ Pi\ 5\ Model\ B.txt
	$(LN) \
		brcmfmac43455-sdio.raspberrypi,4-model-b.txt \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.Raspberry\ Pi\ Foundation-Raspberry\ Pi\ Compute\ Module\ 5.txt
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43455-sdio.MINIX-NEO\ Z83-4.txt \
		$(1)/lib/firmware/brcm/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43455-sdio.AW-CM256SM.txt \
		$(1)/lib/firmware/brcm/
	$(LN) \
		brcmfmac43455-sdio.AW-CM256SM.txt \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.beagle,am5729-beagleboneai.txt
	$(LN) \
		brcmfmac43455-sdio.AW-CM256SM.txt \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.pine64,pinebook-pro.txt
	$(LN) \
		brcmfmac43455-sdio.AW-CM256SM.txt \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.pine64,pinephone-pro.txt
	$(LN) \
		brcmfmac43455-sdio.AW-CM256SM.txt \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.pine64,quartz64-b.txt
endef
$(eval $(call BuildPackage,brcmfmac-nvram-43455-sdio))

Package/brcmfmac-nvram-4356-sdio = $(call Package/firmware-default,Broadcom BCM4356 SDIO NVRAM,,LICENCE.broadcom_bcm43xx)
define Package/brcmfmac-nvram-4356-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac4356-sdio.AP6356S.txt \
		$(1)/lib/firmware/brcm/
	$(LN) \
		brcmfmac4356-sdio.AP6356S.txt \
		$(1)/lib/firmware/brcm/brcmfmac4356-sdio.friendlyarm,nanopc-t4.txt
endef
$(eval $(call BuildPackage,brcmfmac-nvram-4356-sdio))

Package/brcmfmac-firmware-usb = $(call Package/firmware-default,Broadcom BCM43xx fullmac USB firmware,,LICENCE.broadcom_bcm43xx)
define Package/brcmfmac-firmware-usb/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43236b.bin \
		$(1)/lib/firmware/brcm/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43143.bin \
		$(1)/lib/firmware/brcm/
endef
$(eval $(call BuildPackage,brcmfmac-firmware-usb))

Package/brcmsmac-firmware = $(call Package/firmware-default,Broadcom BCM43xx softmac PCIe firmware,,LICENCE.broadcom_bcm43xx)
define Package/brcmsmac-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/brcm/bcm43xx-0.fw \
		$(PKG_BUILD_DIR)/$(PKG_LINUX_FIRMWARE_SUBDIR)/brcm/bcm43xx_hdr-0.fw \
		$(1)/lib/firmware/brcm/
endef
$(eval $(call BuildPackage,brcmsmac-firmware))

Package/bnx2-firmware = $(call Package/firmware-default,Broadcom BCM5706/5708/5709/5716 firmware)
define Package/bnx2-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/bnx2
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bnx2/* \
		$(1)/lib/firmware/bnx2/
endef
$(eval $(call BuildPackage,bnx2-firmware))

Package/bnx2x-firmware = $(call Package/firmware-default,=QLogic 5771x/578xx firmware)
define Package/bnx2x-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/bnx2x
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/bnx2x/* \
		$(1)/lib/firmware/bnx2x/
endef
$(eval $(call BuildPackage,bnx2x-firmware))

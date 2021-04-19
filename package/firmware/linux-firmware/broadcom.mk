Package/brcmfmac-firmware-43602a1-pcie = $(call Package/firmware-default,Broadcom 43602a1 FullMAC PCIe firmware)
define Package/brcmfmac-firmware-43602a1-pcie/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43602-pcie.ap.bin \
		$(1)/lib/firmware/brcm/brcmfmac43602-pcie.bin
endef
$(eval $(call BuildPackage,brcmfmac-firmware-43602a1-pcie))

Package/brcmfmac-firmware-4366b1-pcie = $(call Package/firmware-default,Broadcom 4366b1 FullMAC PCIe firmware)
define Package/brcmfmac-firmware-4366b1-pcie/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac4366b-pcie.bin \
		$(1)/lib/firmware/brcm/
endef
$(eval $(call BuildPackage,brcmfmac-firmware-4366b1-pcie))

Package/brcmfmac-firmware-4366c0-pcie = $(call Package/firmware-default,Broadcom 4366c0 FullMAC PCIe firmware)
define Package/brcmfmac-firmware-4366c0-pcie/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac4366c-pcie.bin \
		$(1)/lib/firmware/brcm/
endef
$(eval $(call BuildPackage,brcmfmac-firmware-4366c0-pcie))

Package/brcmfmac-firmware-4329-sdio = $(call Package/firmware-default,Broadcom BCM4329 FullMac SDIO firmware)
define Package/brcmfmac-firmware-4329-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac4329-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac4329-sdio.bin
endef
$(eval $(call BuildPackage,brcmfmac-firmware-4329-sdio))

Package/brcmfmac-firmware-43430-sdio-rpi-3b = $(call Package/firmware-default,Broadcom BCM43430 NVRAM for Raspberry Pi 3B)
define Package/brcmfmac-firmware-43430-sdio-rpi-3b/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43430-sdio.raspberrypi,3-model-b.txt \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.raspberrypi,3-model-b.txt
endef
$(eval $(call BuildPackage,brcmfmac-firmware-43430-sdio-rpi-3b))

Package/brcmfmac-firmware-43430-sdio-rpi-zero-w = $(call Package/firmware-default,Broadcom BCM43430 NVRAM for Raspberry Pi Zero W)
define Package/brcmfmac-firmware-43430-sdio-rpi-zero-w/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43430-sdio.raspberrypi,3-model-b.txt \
		$(1)/lib/firmware/brcm/brcmfmac43430-sdio.raspberrypi,model-zero-w.txt
endef
$(eval $(call BuildPackage,brcmfmac-firmware-43430-sdio-rpi-zero-w))

Package/brcmfmac-firmware-43430a0-sdio = $(call Package/firmware-default,Broadcom BCM43430a0 FullMac SDIO firmware)
define Package/brcmfmac-firmware-43430a0-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43430a0-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac43430a0-sdio.bin
endef
$(eval $(call BuildPackage,brcmfmac-firmware-43430a0-sdio))

Package/brcmfmac-firmware-43455-sdio-rpi-3b-plus = $(call Package/firmware-default,Broadcom BCM43455 NVRAM for Raspberry Pi 3B+)
define Package/brcmfmac-firmware-43455-sdio-rpi-3b-plus/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43455-sdio.raspberrypi,3-model-b-plus.txt \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.raspberrypi,3-model-b-plus.txt
endef
$(eval $(call BuildPackage,brcmfmac-firmware-43455-sdio-rpi-3b-plus))

Package/brcmfmac-firmware-43455-sdio-rpi-4b = $(call Package/firmware-default,Broadcom BCM43455 NVRAM for Raspberry Pi 4B)
define Package/brcmfmac-firmware-43455-sdio-rpi-4b/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac43455-sdio.raspberrypi,4-model-b.txt \
		$(1)/lib/firmware/brcm/brcmfmac43455-sdio.raspberrypi,4-model-b.txt
endef
$(eval $(call BuildPackage,brcmfmac-firmware-43455-sdio-rpi-4b))

Package/brcmfmac-firmware-4356-sdio = $(call Package/firmware-default,Broadcom BCM4356 FullMac SDIO firmware)
define Package/brcmfmac-firmware-4356-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cypress/cyfmac4356-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac4356-sdio.bin
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cypress/cyfmac4356-sdio.clm_blob \
		$(1)/lib/firmware/brcm/brcmfmac4356-sdio.clm_blob
endef
$(eval $(call BuildPackage,brcmfmac-firmware-4356-sdio))

Package/brcmfmac-firmware-4356-sdio-nanopi-m4 = $(call Package/firmware-default,Broadcom BCM4356 NVRAM for NanoPi M4)
define Package/brcmfmac-firmware-4356-sdio-nanopi-m4/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/brcm/brcmfmac4356-pcie.gpd-win-pocket.txt \
		$(1)/lib/firmware/brcm/brcmfmac4356-sdio.friendlyarm,nanopi-m4.txt
	$(SED) 's/boardrev=.*/boardrev=0x1102/g'\
		$(1)/lib/firmware/brcm/brcmfmac4356-sdio.friendlyarm,nanopi-m4.txt
	$(SED) 's/ccode=.*/ccode=0x5854/g'\
		$(1)/lib/firmware/brcm/brcmfmac4356-sdio.friendlyarm,nanopi-m4.txt
	$(SED) 's/regrev=.*/regrev=205/g'\
		$(1)/lib/firmware/brcm/brcmfmac4356-sdio.friendlyarm,nanopi-m4.txt
	$(SED) 's/devid=.*/devid=0x43ec/g'\
		$(1)/lib/firmware/brcm/brcmfmac4356-sdio.friendlyarm,nanopi-m4.txt
endef
$(eval $(call BuildPackage,brcmfmac-firmware-4356-sdio-nanopi-m4))

Package/brcmfmac-firmware-usb = $(call Package/firmware-default,Broadcom BCM43xx fullmac USB firmware)
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

Package/brcmsmac-firmware = $(call Package/firmware-default,Broadcom BCM43xx softmac PCIe firmware)
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

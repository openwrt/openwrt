Package/cypress-firmware-4339-sdio = $(call Package/firmware-default,Broadcom BCM4339 FullMac SDIO firmware)
define Package/cypress-firmware-4339-sdio/install
	$(INSTALL_DIR) $(1)/lib/firmware/brcm
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/cypress/cyfmac4339-sdio.bin \
		$(1)/lib/firmware/brcm/brcmfmac4339-sdio.bin
endef
$(eval $(call BuildPackage,cypress-firmware-4339-sdio))

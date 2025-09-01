Package/airoha-en8811h-firmware = $(call Package/firmware-default,Airoha EN8811H 2.5G Ethernet PHY firmware,,LICENSE.airoha)
define Package/airoha-en8811h-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/airoha
	$(CP) \
		$(PKG_BUILD_DIR)/airoha/EthMD32.dm.bin \
		$(PKG_BUILD_DIR)/airoha/EthMD32.DSP.bin \
		$(1)/lib/firmware/airoha
ifneq ($(CONFIG_TARGET_mediatek_filogic),)
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	cat \
		$(PKG_BUILD_DIR)/airoha/EthMD32.dm.bin \
		$(PKG_BUILD_DIR)/airoha/EthMD32.DSP.bin \
		> $(STAGING_DIR_IMAGE)/EthMD32.bin
endif
endef

$(eval $(call BuildPackage,airoha-en8811h-firmware))

Package/airoha-en7581-npu-firmware = $(call Package/firmware-default,Airoha EN7581 NPU firmware,,LICENSE.airoha)
define Package/airoha-en7581-npu-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/airoha
	$(CP) \
		$(PKG_BUILD_DIR)/airoha/en7581_npu_data.bin \
		$(PKG_BUILD_DIR)/airoha/en7581_npu_rv32.bin \
		$(1)/lib/firmware/airoha
endef

$(eval $(call BuildPackage,airoha-en7581-npu-firmware))

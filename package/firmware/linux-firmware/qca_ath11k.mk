QCN9074_BOARD_REV:=9372f0a3be311bfaa11a214d039aa0fae5a74756
QCN9074_BOARD_FILE:=board-2.bin.$(QCN9074_BOARD_REV)

define Download/qcn9074-board
  URL:=https://git.codelinaro.org/clo/qsdk/oss/ath11k-bdf/-/raw/NHSS.QSDK.12.5.5/QCN9074/hw1.0/WLAN.HK.2.9.0.1/WLAN.HK.2.9.0.1-01977-QCAHKSWPL_SILICONZ-1/
  URL_FILE:=board-2.bin
  FILE:=$(QCN9074_BOARD_FILE)
  HASH:=2379460b1f9d44bb2d23d50712d6acf04cd067b5fd748043aeba3554190a84e9
endef
$(eval $(call Download,qcn9074-board))

Package/ath11k-firmware-ipq5018 = $(call Package/firmware-default,IPQ5018 ath11k firmware,,LICENCE.atheros_firmware)
define Package/ath11k-firmware-ipq5018/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/IPQ5018/hw1.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ath11k/IPQ5018/hw1.0/* $(1)/lib/firmware/ath11k/IPQ5018/hw1.0/
endef
$(eval $(call BuildPackage,ath11k-firmware-ipq5018))

Package/ath11k-firmware-ipq8074 = $(call Package/firmware-default,IPQ8074 ath11k firmware,,LICENCE.atheros_firmware)
define Package/ath11k-firmware-ipq8074/install
	$(INSTALL_DIR) $(1)/lib/firmware/IPQ8074
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ath11k/IPQ8074/hw2.0/* $(1)/lib/firmware/IPQ8074/
endef
$(eval $(call BuildPackage,ath11k-firmware-ipq8074))

Package/ath11k-firmware-qca2066 = $(call Package/firmware-default,QCA2066 ath11k firmware,,LICENCE.atheros_firmware)
define Package/ath11k-firmware-qca2066/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/QCA2066/hw2.1
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ath11k/QCA2066/hw2.1/* $(1)/lib/firmware/ath11k/QCA2066/hw2.1/
endef
$(eval $(call BuildPackage,ath11k-firmware-qca2066))

Package/ath11k-firmware-qca6390 = $(call Package/firmware-default,QCA6390 ath11k firmware,,LICENCE.atheros_firmware)
define Package/ath11k-firmware-qca6390/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/QCA6390/hw2.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ath11k/QCA6390/hw2.0/* $(1)/lib/firmware/ath11k/QCA6390/hw2.0/
endef
$(eval $(call BuildPackage,ath11k-firmware-qca6390))

Package/ath11k-firmware-qcn9074 = $(call Package/firmware-default,QCN9074 ath11k firmware,,LICENCE.atheros_firmware)
define Package/ath11k-firmware-qcn9074/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/QCN9074/hw1.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ath11k/QCN9074/hw1.0/* $(1)/lib/firmware/ath11k/QCN9074/hw1.0/
	$(INSTALL_BIN) \
		$(DL_DIR)/$(QCN9074_BOARD_FILE) $(1)/lib/firmware/ath11k/QCN9074/hw1.0/board-2.bin
endef
$(eval $(call BuildPackage,ath11k-firmware-qcn9074))

Package/ath11k-firmware-wcn6750 = $(call Package/firmware-default,WCN6750 ath11k firmware,,LICENCE.atheros_firmware)
define Package/ath11k-firmware-wcn6750/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/WCN6750/hw1.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ath11k/WCN6750/hw1.0/board-2.bin $(1)/lib/firmware/ath11k/WCN6750/hw1.0/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ath11k/WCN6750/hw1.0/Notice.txt $(1)/lib/firmware/ath11k/WCN6750/hw1.0/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ath11k/WCN6750/hw1.0/sc7280/wpss.mbn $(1)/lib/firmware/ath11k/WCN6750/hw1.0/
endef
$(eval $(call BuildPackage,ath11k-firmware-wcn6750))

Package/ath11k-firmware-wcn6855 = $(call Package/firmware-default,WCN6855 ath11k firmware,,LICENCE.atheros_firmware)
define Package/ath11k-firmware-wcn6855/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/WCN6855/hw2.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ath11k/WCN6855/hw2.0/*.bin $(1)/lib/firmware/ath11k/WCN6855/hw2.0/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ath11k/WCN6855/hw2.0/*.txt $(1)/lib/firmware/ath11k/WCN6855/hw2.0/
	$(LN) ./hw2.0 $(1)/lib/firmware/ath11k/WCN6855/hw2.1
endef
$(eval $(call BuildPackage,ath11k-firmware-wcn6855))

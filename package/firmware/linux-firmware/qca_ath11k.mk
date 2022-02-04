Package/ath11k-board-qca6390 = $(call Package/firmware-default,ath10k qca6390 board firmware)
define Package/ath11k-board-qca6390/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/qca6390/hw2.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ath11k/qca6390/hw2.0/board-2.bin \
		$(1)/lib/firmware/ath11k/qca6390/hw2.0/
endef
$(eval $(call BuildPackage,ath11k-board-qca6390))
Package/ath11k-firmware-qca6390 = $(call Package/firmware-default,ath11k qca6390 firmware,+ath11k-board-qca6390)
define Package/ath11k-firmware-qca6390/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/qca6390/hw2.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ath11k/qca6390/hw2.0/amss.bin \
		$(PKG_BUILD_DIR)/ath11k/qca6390/hw2.0/m3.bin \
		$(1)/lib/firmware/ath11k/qca6390/hw2.0
endef
$(eval $(call BuildPackage,ath11k-firmware-qca6390))

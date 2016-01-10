Package/r8169-firmware = $(call Package/firmware-default,RealTek RTL8169 firmware)
define Package/r8169-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/rtl_nic
	$(CP) \
		$(PKG_BUILD_DIR)/rtl_nic/* \
		$(1)/lib/firmware/rtl_nic
endef
$(eval $(call BuildPackage,r8169-firmware))

Package/r8188eu-firmware = $(call Package/firmware-default,RealTek RTL8188EU firmware)
define Package/r8188eu-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/rtlwifi
	$(CP) \
		$(PKG_BUILD_DIR)/rtlwifi/rtl8188eufw.bin \
		$(1)/lib/firmware/rtlwifi
endef
$(eval $(call BuildPackage,r8188eu-firmware))

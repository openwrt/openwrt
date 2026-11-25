Package/mscc-vsc85xx-firmware = $(call Package/firmware-default,Microchip VSC85xx Ethernet PHY 8051 patches,,LICENCE.microchip)
define Package/mscc-vsc85xx-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/microchip
	$(CP) \
		$(PKG_BUILD_DIR)/microchip/mscc_vsc8574_revb_int8051_29e8.bin \
		$(PKG_BUILD_DIR)/microchip/mscc_vsc8584_revb_int8051_fb48.bin \
		$(1)/lib/firmware/microchip
endef

$(eval $(call BuildPackage,mscc-vsc85xx-firmware))

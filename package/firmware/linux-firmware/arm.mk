Package/panthor-firmware = $(call Package/firmware-default,ARM Mali CSF GPU firmware,,LICENCE.mali_csffw)
define Package/panthor-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/arm/mali
	$(CP) $(PKG_BUILD_DIR)/arm/mali/arch*/ $(1)/lib/firmware/arm/mali/
endef
$(eval $(call BuildPackage,panthor-firmware))

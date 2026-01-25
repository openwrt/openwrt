Package/amd64-microcode = $(call Package/firmware-default,AMD64 CPU microcode,,LICENSE.amd-ucode)
define Package/amd64-microcode/install
	$(INSTALL_DIR) $(1)/boot
	mkdir -p $(PKG_BUILD_DIR)/kernel/x86/microcode
	cat $(PKG_BUILD_DIR)/amd-ucode/microcode_amd*.bin \
		> $(PKG_BUILD_DIR)/kernel/x86/microcode/AuthenticAMD.bin
	( cd $(PKG_BUILD_DIR) ; echo "kernel/x86/microcode/AuthenticAMD.bin" | \
		cpio -o -H newc --reproducible > $(1)/boot/amd-ucode.img )
endef

$(eval $(call BuildPackage,amd64-microcode))

Package/amdgpu-firmware = $(call Package/firmware-default,AMDGPU Video Driver firmware,,LICENSE.amdgpura)
define Package/amdgpu-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/amdgpu
	$(CP) \
		$(PKG_BUILD_DIR)/amdgpu/*.bin \
		$(1)/lib/firmware/amdgpu
endef

$(eval $(call BuildPackage,amdgpu-firmware))

Package/radeon-firmware = $(call Package/firmware-default,Radeon Video Driver firmware,,LICENSE.radeon)
define Package/radeon-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/radeon
	$(CP) \
		$(PKG_BUILD_DIR)/radeon/*.bin \
		$(1)/lib/firmware/radeon
endef

$(eval $(call BuildPackage,radeon-firmware))

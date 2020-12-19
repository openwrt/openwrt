include $(TOPDIR)/rules.mk

PKG_NAME:=wasp_uploader
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/wasp_uploader
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=AVM WASP firmware uploader
  URL:=https://github.com/andy/boeh/wasp_uploader.git
endef

define Package/wasp_uploader/description
   Upload firmware to AVM WASP devices
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/wasp_uploader/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wasp_uploader_stage1 $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wasp_uploader_stage2 $(1)/usr/bin/
	[ -d $(PKG_BUILD_DIR)/files ] && $(CP) $(PKG_BUILD_DIR)/files/* $(1)/
	$(INSTALL_DIR) $(1)/opt/wasp
	#$(CP) $(PKG_BUILD_DIR)/files/opt/wasp/upload-wasp.sh $(1)/opt/wasp/
	#$(CP) $(PKG_BUILD_DIR)/files/etc/init.d/upload-wasp $(1)/etc/init.d/
	$(CP) $(STAGING_DIR_HOST)/squashfs-root/lib/firmware/ath_tgt_fw1.fw $(1)/opt/wasp/
	$(INSTALL_DIR) $(1)/lib/firmware
	$(CP) $(STAGING_DIR_HOST)/squashfs-root/lib/firmware/xhcifw.mem $(1)/lib/firmware/K2026090.mem
	$(CP) $(STAGING_DIR_HOST)/squashfs-root/lib/modules/dsp_vr9/vr9-B-dsl.bin $(1)/lib/firmware/vr9_dsl_fw_annex_b.bin
	bspatch $(STAGING_DIR_HOST)/squashfs-root/lib/modules/dsp_vr9/vr9-B-dsl.bin \
		$(STAGING_DIR_HOST)/squashfs-root/lib/modules/dsp_vr9/vr9-A-dsl.bin \
		$(STAGING_DIR_HOST)/squashfs-root/lib/modules/dsp_vr9/vr9-A-dsl.bin.bsdiff
	#$(CP) $(STAGING_DIR_HOST)/squashfs-root/lib/modules/dsp_vr9/vr9-A-dsl.bin $(1)/lib/firmware/vr9_dsl_fw_annex_a.bin
	$(CP) $(BIN_DIR)/../../ath79/generic/openwrt-ath79-generic-avm_fritz7490_wasp-initramfs-kernel.bin $(1)/opt/wasp
endef

$(eval $(call BuildPackage,wasp_uploader))

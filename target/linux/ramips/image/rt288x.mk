#
# RT288X Profiles
#

# Airlink101 AR670W -- LZMA without uImage header
define BuildFirmware/AR670W/squashfs
	$(call PatchKernelLzmaDtb,$(2),$(3),$(5))

	$(eval output_name=$(IMG_PREFIX)-$(2)-$(1)-$(if $(4),$(4),sysupgrade).bin)
	$(eval factory_name=$(IMG_PREFIX)-$(2)-$(1)-factory.bin)

	( dd if=$(KDIR)/vmlinux-$(2).bin.lzma bs=65536 \
		conv=sync; dd if=$(KDIR)/root.$(1) ) > $(KDIR)/$(output_name)

	$(call prepare_generic_squashfs,$(KDIR)/$(output_name))

	if [ `stat -c%s "$(KDIR)/$(output_name)"` \
		-gt $(ralink_default_fw_size_4M) ]; then \
		echo "Warning: $(KDIR)/$(output_name) is too big" >&2; \
	else \
		mkwrgimg -i $(KDIR)/$(output_name) \
			-d "/dev/mtdblock/2" \
			-s "wrgn16a_airlink_ar670w" \
			-o $(BIN_DIR)/$(factory_name); \
		$(CP) $(KDIR)/$(output_name) $(BIN_DIR)/$(output_name); \
	fi
endef

define BuildFirmware/Gemtek/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),$(2),$(3))
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		mkheader_gemtek "$(call sysupname,$(1),$(2))" \
			"$(call imgname,$(1),$(2))-factory.bin" $(2) || \
		rm -f "$(call imgname,$(1),$(2))-factory.bin"; \
	fi
endef
BuildFirmware/Gemtek/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))


Image/Build/Profile/AR670W=$(call BuildFirmware/AR670W/$(1),$(1),ar670w,AR670W)
Image/Build/Profile/AR725W=$(call BuildFirmware/Gemtek/$(1),$(1),ar725w,AR725W)
# 0x790000
belkin_f5d8235v1_mtd_size=7929856
Image/Build/Profile/F5D8235V1=$(call BuildFirmware/CustomFlash/$(1),$(1),f5d8235v1,F5D8235_V1,$(belkin_f5d8235v1_mtd_size))
Image/Build/Profile/RTN15=$(call BuildFirmware/Default4M/$(1),$(1),rt-n15,RT-N15)
Image/Build/Profile/V11STFE=$(call BuildFirmware/Default4M/$(1),$(1),v11st-fe,V11STFE)
Image/Build/Profile/WLITX4AG300N=$(call BuildFirmware/Default4M/$(1),$(1),wli-tx4-ag300n,WLI-TX4-AG300N)
Image/Build/Profile/WZRAGL300NH=$(call BuildFirmware/Default4M/$(1),$(1),wzr-agl300nh,WZR-AGL300NH)


define Image/Build/Profile/Default
	$(call Image/Build/Profile/AR670W,$(1))
	$(call Image/Build/Profile/AR725W,$(1))
	$(call Image/Build/Profile/F5D8235V1,$(1))
	$(call Image/Build/Profile/RTN15,$(1))
	$(call Image/Build/Profile/V11STFE,$(1))
	$(call Image/Build/Profile/WLITX4AG300N,$(1))
	$(call Image/Build/Profile/WZRAGL300NH,$(1))
endef

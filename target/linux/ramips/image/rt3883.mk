#
# RT3662/RT3883 Profiles
#

define Device/TEW691GR
  DTS := TEW-691GR
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(IMAGE/sysupgrade.bin) | umedia-header 0x026910
  DEVICE_TITLE := TRENDnet TEW-691GR
  DEVICE_PACKAGES := swconfig
endef
TARGET_DEVICES += TEW691GR


define Device/TEW692GR
  DTS := TEW-692GR
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(IMAGE/sysupgrade.bin) | umedia-header 0x026920
  DEVICE_TITLE := TRENDnet TEW-692GR
  DEVICE_PACKAGES := swconfig
endef
TARGET_DEVICES += TEW692GR


# $(1) = squashfs/initramfs
# $(2) = lowercase board name
# $(3) = dts file
# $(4) = kernel size
# $(5) = rootfs size
# $(6) = signature
# $(7) = model
# $(8) = flash address
# $(9) = start address
define BuildFirmware/EdimaxCombined/squashfs
	$(call PatchKernelLzmaDtb,$(2),$(3))
	$(call MkImage,lzma,$(KDIR)/vmlinux-$(2).bin.lzma,$(KDIR)/vmlinux-$(2).bin.uImage)
	if [ `stat -c%s "$(KDIR)/vmlinux-$(2).bin.uImage"` -gt `expr $(4) - 20` ]; then \
		echo "Warning: $(KDIR)/vmlinux-$(2).bin.uImage is too big" >&2; \
		exit 1; \
	else if [ `stat -c%s "$(KDIR)/root.$(1)"` -gt $(5) ]; then \
		echo "Warning: $(KDIR)/root.$(1) is too big" >&2; \
		exit 1; \
	else \
		( dd if=$(KDIR)/vmlinux-$(2).bin.uImage bs=`expr $(4) - 20` conv=sync ; dd if=$(KDIR)/root.$(1) ) > $(KDIR)/vmlinux-$(2).bin.uImage.combined ; \
	fi ; fi
	mkedimaximg -i "$(KDIR)/vmlinux-$(2).bin.uImage.combined" \
		-o $(call imgname,$(1),$(2)).bin \
		-s $(6) -m $(7) -f $(8) -S $(9)
endef

define BuildFirmware/RTN56U/squashfs
	$(call BuildFirmware/Default8M/$(1),$(1),rt-n56u,RT-N56U)
	-mkrtn56uimg -s $(call sysupname,$(1),rt-n56u)
	-cp $(call sysupname,$(1),rt-n56u) $(call imgname,$(1),rt-n56u)-factory.bin
	-mkrtn56uimg -f $(call imgname,$(1),rt-n56u)-factory.bin
endef


Image/Build/Profile/CYSWR1100=$(call BuildFirmware/Seama/$(1),$(1),cy-swr1100,CY-SWR1100,wrgnd10_samsung_ss815,$(ralink_default_fw_size_8M))
Image/Build/Profile/DIR645=$(call BuildFirmware/Seama/$(1),$(1),dir-645,DIR-645,wrgn39_dlob.hans_dir645,$(ralink_default_fw_size_8M))
hpm_mtd_size=16449536
Image/Build/Profile/HPM=$(call BuildFirmware/CustomFlash/$(1),$(1),hpm,HPM,$(hpm_mtd_size))
Image/Build/Profile/RTN56U=$(call BuildFirmware/RTN56U/$(1),$(1),rt-n56u,RT-N56U)
kernel_size_BR6475ND:=2097152
rootfs_size_BR6475ND:=5832704
Image/Build/Profile/BR6475ND=$(call BuildFirmware/EdimaxCombined/$(1),$(1),br-6475nd,BR-6475ND,$(kernel_size_BR6475ND),$(rootfs_size_BR6475ND),CSYS,RN54,0x70000,0x01100000)


define LegacyDevice/CYSWR1100
  DEVICE_TITLE := Samsung CY-SWR1100
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 swconfig
endef
LEGACY_DEVICES += CYSWR1100


define LegacyDevice/DIR645
  DEVICE_TITLE := D-Link DIR-645
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 swconfig
endef
LEGACY_DEVICES += DIR645


define LegacyDevice/HPM
  DEVICE_TITLE := Omnima HPM
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2
endef
LEGACY_DEVICES += HPM


define LegacyDevice/RTN56U
  DEVICE_TITLE := Asus RT-N56U
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 swconfig
endef
LEGACY_DEVICES += RTN56U


define LegacyDevice/BR6475ND
  DEVICE_TITLE := Edimax BR-6475nD
  DEVICE_PACKAGES := swconfig
endef
LEGACY_DEVICES += BR6475ND

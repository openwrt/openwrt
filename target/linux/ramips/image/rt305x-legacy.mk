#
# RT305X Profiles
#
kernel_size_wl341v3=917504
rootfs_size_wl341v3=2949120
define BuildFirmware/WL-341V3/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),wl-341v3,WL-341V3)
	# This code looks broken and really needs to be converted to C
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		( \
			dd if=/dev/zero bs=195936 count=1; \
			echo "1.01"; \
			dd if=/dev/zero bs=581 count=1; \
			echo -n -e "\x73\x45\x72\x43\x6F\x4D\x6D\x00\x01\x00\x00\x59\x4E\x37\x95\x58\x10\x00\x20\x00\x28\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x11\x03\x00\x00\x80\x00\x00\x00\x00\x03\x00\x00\x10\x12\x00\x00\x00\x10\x73\x45\x72\x43\x6F\x4D\x6D"; \
			dd if=/dev/zero bs=65552 count=1; \
			dd if=$(KDIR)/vmlinux-$(2).uImage bs=$(kernel_size_wl341v3) conv=sync; \
			dd if=$(KDIR)/root.$(1) bs=64k conv=sync; \
			dd if=/dev/zero bs=`expr 4194304 - 262144 - 16 - $(kernel_size_wl341v3) - \( \( \( \`stat -c%s $(KDIR)/root.$(1)\` / 65536 \) + 1 \) \* 65536 \)` count=1; \
			echo -n -e "\x11\x03\x80\x00\x10\x12\x90\xF7\x65\x52\x63\x4F\x6D\x4D\x00\x00"; \
		) > $(call imgname,$(1),$(2))-factory.bin; \
	fi
endef
BuildFirmware/WL-341V3/initramfs=$(call BuildFirmware/OF/initramfs,$(1),wl-341v3,WL-341V3)

define BuildFirmware/UIMAGE_8M
	$(call MkCombineduImage,$(1),$(2),$(call mkcmdline,$(3),$(4),$(5)) $(call mkmtd/$(6),$(mtdlayout_8M)),$(kernel_size_8M),$(rootfs_size_8M),$(7))
endef
define Image/Build/Profile/ALL02393G
	$(call Image/Build/Template/$(image_type)/$(1),UIMAGE_8M,all0239-3g,ALL0239-3G,ttyS1,57600,phys)
endef

Image/Build/Profile/WL-341V3=$(call BuildFirmware/WL-341V3/$(1),$(1))

define LegacyDevice/ALL02393G
  DEVICE_TITLE := Allnet ALL0239-3G 
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-dwc2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += ALL02393G

define LegacyDevice/WL-341V3
  DEVICE_TITLE := Sitecom WL-341 v3
endef
LEGACY_DEVICES += WL-341V3

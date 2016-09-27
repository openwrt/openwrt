#
# RT305X Profiles
#

# Sign Poray images
define BuildFirmware/Poray4M/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),$(2),$(3))
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		mkporayfw -B $(3) -F 4M \
			-f $(call sysupname,$(1),$(2)) \
			-o $(call imgname,$(1),$(2))-factory.bin; \
	fi
endef
BuildFirmware/Poray4M/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

define BuildFirmware/Poray8M/squashfs
	$(call BuildFirmware/Default8M/$(1),$(1),$(2),$(3))
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		mkporayfw -B $(3) -F 8M \
			-f $(call sysupname,$(1),$(2)) \
			-o $(call imgname,$(1),$(2))-factory.bin; \
	fi
endef
BuildFirmware/Poray8M/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

define BuildFirmware/JCG/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),$(2),$(3))
	-jcgimage -o $(call imgname,$(1),$(2))-factory.bin \
		-u $(call sysupname,$(1),$(2)) -v $(4)
endef

define BuildFirmware/DIR300B1/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),$(2),$(3))
	-mkwrgimg -s $(4) -d /dev/mtdblock/2 \
		-i $(call sysupname,$(1),$(2)) \
		-o $(call imgname,$(1),$(2))-factory.bin
endef
BuildFirmware/DIR300B1/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

# sign dap 1350 based images
dap1350_mtd_size=7667712
define BuildFirmware/dap1350/squashfs
	$(call BuildFirmware/CustomFlash/$(1),$(1),$(2),$(3),$(dap1350_mtd_size))
	-mkdapimg -s $(4) \
		-i $(call sysupname,$(1),$(2)) \
		-o $(call imgname,$(1),$(2))-factory.bin
endef
BuildFirmware/dap1350/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

define BuildFirmware/DCS930/squashfs
	$(call BuildFirmware/Default4M/squashfs,squashfs,$(2),$(3))
	dd if=$(KDIR)/vmlinux-$(2).bin.lzma of=$(KDIR)/image.$(2).combined bs=1048512 count=1 conv=sync
	cat $(KDIR)/root.squashfs >> $(KDIR)/image.$(2).combined
	$(call MkImage,lzma,$(KDIR)/image.$(2).combined,$(KDIR)/image.$(2))
	$(call prepare_generic_squashfs,$(KDIR)/image.$(2))
	if [ `stat -c%s "$(KDIR)/image.$(2)"` -gt $(ralink_default_fw_size_4M) ]; then \
		echo "Warning: $(KDIR)/image.$(2)  is too big" >&2; \
	else \
		dd if=$(KDIR)/image.$(2) of=$(KDIR)/dcs.tmp bs=64K count=5 conv=sync ;\
		cat $(KDIR)/image.$(2) >> $(KDIR)/dcs.tmp ; \
		dd if=$(KDIR)/dcs.tmp of=$(call imgname,$(1),$(2))-factory.bin bs=4096k count=1 conv=sync ;\
		$(STAGING_DIR_HOST)/bin/mkdcs932 $(call imgname,$(1),$(2))-factory.bin ; \
	fi
endef
BuildFirmware/DCS930/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

BuildFirmware/HLKRM04/squashfs=$(call BuildFirmware/Default4M/squashfs,$(1),$(2),$(3),$(4))
define BuildFirmware/HLKRM04/initramfs
	$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3),$(4))
	mkhilinkfw -e -i$(call imgname,$(1),$(2))-uImage.bin -o $(call imgname,$(1),$(2))-factory.bin;
endef

# sign Buffalo images
define BuildFirmware/Buffalo
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		buffalo-enc -p $(3) -v 1.76 \
			-i $(KDIR)/vmlinux-$(2).uImage \
			-o $(KDIR)/vmlinux-$(2).uImage.enc; \
		buffalo-enc -p $(3) -v 1.76 \
			-i $(KDIR)/root.$(1) \
			-o $(KDIR)/root.$(2).enc; \
		buffalo-tag -b $(3) -p $(3) -a ram -v 1.76 -m 1.01 \
			-l mlang8 -f 1 -r EU \
			-i $(KDIR)/vmlinux-$(2).uImage.enc \
			-i $(KDIR)/root.$(2).enc \
			-o $(call imgname,$(1),$(2))-factory-EU.bin; \
	fi
endef

# FIXME: this looks broken
buffalo_whrg300n_mtd_size=3801088
define BuildFirmware/WHRG300N/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),whr-g300n,WHR-G300N)
	# the following line has a bad argument 3 ... the old Makefile was already broken	
	$(call BuildFirmware/Buffalo,$(1),whr-g300n,whr-g300n)
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		( \
			echo -n -e "# Airstation FirmWare\nrun u_fw\nreset\n\n" | \
				dd bs=512 count=1 conv=sync; \
			dd if=$(call sysupname,$(1),whr-g300n); \
		) > $(KDIR)/whr-g300n-tftp.tmp && \
		buffalo-tftp -i $(KDIR)/whr-g300n-tftp.tmp \
			-o $(call imgname,$(1),whr-g300n)-tftp.bin; \
	fi
endef
BuildFirmware/WHRG300N/initramfs=$(call BuildFirmware/OF/initramfs,$(1),whr-g300n,WHR-G300N)

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

define BuildFirmware/WNCE2001/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),$(2),$(3))
	-mkdapimg -s RT3052-AP-WNCE2001-3 -r WW -v 1.0.0.99 \
		-i $(call sysupname,$(1),$(2)) \
		-o $(call imgname,$(1),$(2))-factory-worldwide.bin
	-mkdapimg -s RT3052-AP-WNCE2001-3 -r NA -v 1.0.0.99 \
		-i $(call sysupname,$(1),$(2)) \
		-o $(call imgname,$(1),$(2))-factory-northamerica.bin
endef
BuildFirmware/WNCE2001/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))



define BuildFirmware/UIMAGE_8M
	$(call MkCombineduImage,$(1),$(2),$(call mkcmdline,$(3),$(4),$(5)) $(call mkmtd/$(6),$(mtdlayout_8M)),$(kernel_size_8M),$(rootfs_size_8M),$(7))
endef
define Image/Build/Profile/ALL02393G
	$(call Image/Build/Template/$(image_type)/$(1),UIMAGE_8M,all0239-3g,ALL0239-3G,ttyS1,57600,phys)
endef

Image/Build/Profile/3G300M=$(call BuildFirmware/CustomFlashFactory/$(1),$(1),3g300m,3G300M,$(ralink_default_fw_size_4M),3G150M_SPI Kernel Image,factory)
Image/Build/Profile/A5-V11=$(call BuildFirmware/Poray4M/$(1),$(1),a5-v11,A5-V11)
Image/Build/Profile/ALL0256N=$(call BuildFirmware/DefaultDualSize/$(1),$(1),all0256n,ALL0256N)
Image/Build/Profile/AWM002EVB=$(call BuildFirmware/DefaultDualSize/$(1),$(1),awm002-evb,AWM002-EVB)
broadway_mtd_size=7929856
Image/Build/Profile/BROADWAY=$(call BuildFirmware/CustomFlashFactory/$(1),$(1),broadway,BROADWAY,$(broadway_mtd_size),Broadway Kernel Image,factory)
Image/Build/Profile/JHR-N805R=$(call BuildFirmware/JCG/$(1),$(1),jhr-n805r,JHR-N805R,29.24)
Image/Build/Profile/JHR-N825R=$(call BuildFirmware/JCG/$(1),$(1),jhr-n825r,JHR-N825R,23.24)
Image/Build/Profile/JHR-N926R=$(call BuildFirmware/JCG/$(1),$(1),jhr-n926r,JHR-N926R,25.24)
Image/Build/Profile/DIR-300-B1=$(call BuildFirmware/DIR300B1/$(1),$(1),dir-300-b1,DIR-300-B1,wrgn23_dlwbr_dir300b)
Image/Build/Profile/DIR-600-B1=$(call BuildFirmware/DIR300B1/$(1),$(1),dir-600-b1,DIR-600-B1,wrgn23_dlwbr_dir600b)
Image/Build/Profile/DIR-600-B2=$(call BuildFirmware/DIR300B1/$(1),$(1),dir-600-b2,DIR-600-B2,wrgn23_dlwbr_dir600b)
Image/Build/Profile/DIR-615-D=$(call BuildFirmware/DIR300B1/$(1),$(1),dir-615-d,DIR-615-D,wrgn23_dlwbr_dir615d)
Image/Build/Profile/DAP1350=$(call BuildFirmware/dap1350/$(1),$(1),dap-1350,DAP-1350,RT3052-AP-DAP1350-3)
Image/Build/Profile/DAP1350WW=$(call BuildFirmware/dap1350/$(1),$(1),dap-1350WW,DAP-1350,RT3052-AP-DAP1350WW-3)
Image/Build/Profile/DCS930=$(call BuildFirmware/DCS930/$(1),$(1),dcs-930,DCS-930)
Image/Build/Profile/DCS930LB1=$(call BuildFirmware/DCS930/$(1),$(1),dcs-930l-b1,DCS-930L-B1)
Image/Build/Profile/HLKRM04=$(call BuildFirmware/HLKRM04/$(1),$(1),hlk-rm04,HLKRM04,HLK-RM04)
Image/Build/Profile/M3=$(call BuildFirmware/Poray4M/$(1),$(1),m3,M3)
Image/Build/Profile/M4=$(call BuildFirmware/PorayDualSize/$(1),$(1),m4,M4)
nw718_mtd_size=3801088
Image/Build/Profile/NW718=$(call BuildFirmware/CustomFlashFactory/$(1),$(1),nw718m,NW718,$(nw718_mtd_size),ARA1B4NCRNW718;1,factory)
Image/Build/Profile/PX-4885=$(call BuildFirmware/DefaultDualSize/$(1),$(1),px-4885,PX-4885)
Image/Build/Profile/W150M=$(call BuildFirmware/CustomFlashFactory/$(1),$(1),w150m,W150M,$(ralink_default_fw_size_4M),W150M Kernel Image,factory)
Image/Build/Profile/W306R_V20=$(call BuildFirmware/CustomFlashFactory/$(1),$(1),w306r-v20,W306R_V20,$(ralink_default_fw_size_4M),linkn Kernel Image,factory)
Image/Build/Profile/WL-341V3=$(call BuildFirmware/WL-341V3/$(1),$(1))
Image/Build/Profile/WNCE2001=$(call BuildFirmware/WNCE2001/$(1),$(1),wnce2001,WNCE2001)
Image/Build/Profile/WR512-3GN=$(call BuildFirmware/DefaultDualSize/$(1),$(1),wr512-3ng,WR512-3GN)
Image/Build/Profile/WT1520=$(call BuildFirmware/PorayDualSize/$(1),$(1),wt1520,WT1520)
Image/Build/Profile/X5=$(call BuildFirmware/Poray8M/$(1),$(1),x5,X5)
Image/Build/Profile/X8=$(call BuildFirmware/Poray8M/$(1),$(1),x8,X8)
Image/Build/Profile/WHRG300N=$(call BuildFirmware/WHRG300N/$(1),$(1))

define LegacyDevice/ALL02393G
  DEVICE_TITLE := Allnet ALL0239-3G 
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-dwc2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += ALL02393G


define Device/dir-610-a1
  DTS := DIR-610-A1
  BLOCKSIZE := 4k
  IMAGES += factory.bin
  KERNEL := $(KERNEL_DTB)
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  IMAGE/sysupgrade.bin := \
	append-kernel | pad-offset $$$$(BLOCKSIZE) 64 | append-rootfs | \
	seama -m "dev=/dev/mtdblock/2" -m "type=firmware" | \
	pad-rootfs | check-size $$$$(IMAGE_SIZE)
  IMAGE/factory.bin := \
	append-kernel | pad-offset $$$$(BLOCKSIZE) 64 | \
	append-rootfs | pad-rootfs -x 64 | \
	seama -m "dev=/dev/mtdblock/2" -m "type=firmware" | \
	seama-seal -m "signature=wrgn59_dlob.hans_dir610" | \
	check-size $$$$(IMAGE_SIZE)
  DEVICE_TITLE := D-Link DIR-610 A1 
  DEVICE_PACKAGES := kmod-ledtrig-netdev kmod-ledtrig-timer
endef
TARGET_DEVICES += dir-610-a1


define Device/3g-6200n
  DTS := 3G-6200N
  IMAGE_SIZE := 3648k
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | \
	edimax-header -s CSYS -m 3G62 -f 0x50000 -S 0x01100000 | pad-rootfs
  DEVICE_TITLE := Edimax 3g-6200n
endef
TARGET_DEVICES += 3g-6200n


define Device/3g-6200nl
  DTS := 3G-6200NL
  IMAGE_SIZE := 3648k
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | \
	edimax-header -s CSYS -m 3G62 -f 0x50000 -S 0x01100000 | pad-rootfs
  DEVICE_TITLE := Edimax 3g-6200nl
endef
TARGET_DEVICES += 3g-6200nl


define LegacyDevice/3G300M
  DEVICE_TITLE := Tenda 3G300M
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-dwc2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += 3G300M


define LegacyDevice/A5-V11
  DEVICE_TITLE := A5-V11
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2
endef
LEGACY_DEVICES += A5-V11


define LegacyDevice/ALL0256N
  DEVICE_TITLE := Allnet ALL0256N
  DEVICE_PACKAGES := rssileds
endef
LEGACY_DEVICES += ALL0256N


define LegacyDevice/AWM002EVB
  DEVICE_TITLE := AsiaRF AWM002-EVB
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev \
		kmod-i2c-core kmod-i2c-gpio
endef
LEGACY_DEVICES += AWM002EVB


define LegacyDevice/BROADWAY
  DEVICE_TITLE := Hauppauge Broadway
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-dwc2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += BROADWAY


define LegacyDevice/JHR-N805R
  DEVICE_TITLE := JCG JHR-N805R
endef
LEGACY_DEVICES += JHR-N805R


define LegacyDevice/JHR-N825R
  DEVICE_TITLE := JCG JHR-N825R
endef
LEGACY_DEVICES += JHR-N825R


define LegacyDevice/JHR-N926R
  DEVICE_TITLE := JCG JHR-N926R
endef
LEGACY_DEVICES += JHR-N926R


define LegacyDevice/DIR-300-B1
  DEVICE_TITLE := D-Link DIR-300 B1
endef
LEGACY_DEVICES += DIR-300-B1


define LegacyDevice/DIR-600-B1
  DEVICE_TITLE := D-Link DIR-600 B1
endef
LEGACY_DEVICES += DIR-600-B1


define LegacyDevice/DIR-600-B2
  DEVICE_TITLE := D-Link DIR-600 B2
endef
LEGACY_DEVICES += DIR-600-B2


define LegacyDevice/DIR-615-D
  DEVICE_TITLE := D-Link DIR-615 D
endef
LEGACY_DEVICES += DIR-615-D


define Device/dir-615-h1
  DTS := DIR-615-H1
  BLOCKSIZE := 4k
  IMAGES += factory.bin
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  IMAGE/factory.bin := \
	$$(IMAGE/sysupgrade.bin) | senao-header -r 0x218 -p 0x30 -t 3
  DEVICE_TITLE := D-Link DIR-615 H1
endef
TARGET_DEVICES += dir-615-h1


define LegacyDevice/DAP1350
  DEVICE_TITLE := D-Link DAP-1350
endef
LEGACY_DEVICES += DAP1350


define LegacyDevice/DAP1350WW
  DEVICE_TITLE := D-Link DAP-1350HW
endef
LEGACY_DEVICES += DAP1350WW


define LegacyDevice/DCS930
  DEVICE_TITLE := D-Link DCS-930
  DEVICE_PACKAGES := kmod-video-core kmod-video-uvc kmod-sound-core kmod-usb-audio kmod-usb-core kmod-usb-dwc2
endef
LEGACY_DEVICES += DCS930


define LegacyDevice/DCS930LB1
  DEVICE_TITLE := D-Link DCS-930L B1
  DEVICE_PACKAGES := kmod-video-core kmod-video-uvc kmod-sound-core kmod-usb-audio kmod-usb-core kmod-usb-ohci kmod-usb2
endef
LEGACY_DEVICES += DCS930LB1


define Device/fonera20n
  DTS := FONERA20N
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(IMAGE/sysupgrade.bin) | \
	edimax-header -s RSDK -m NL1T -f 0x50000 -S 0xc0000
  DEVICE_TITLE := Fonera 2.0N
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-dwc2 kmod-ledtrig-usbdev
endef
TARGET_DEVICES += fonera20n


define LegacyDevice/HLKRM04
  DEVICE_TITLE := HILINK HLK-RM04
endef
LEGACY_DEVICES += HLKRM04


define LegacyDevice/M3
  DEVICE_TITLE := Poray M3
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-netdev kmod-ledtrig-timer
endef
LEGACY_DEVICES += M3


define LegacyDevice/M4
  DEVICE_TITLE := Poray M4
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-netdev kmod-ledtrig-timer
endef
LEGACY_DEVICES += M4


define Device/mzk-w300nh2
  DTS := MZK-W300NH2
  IMAGE_SIZE := 3648k
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(IMAGE/sysupgrade.bin) | \
	edimax-header -s CSYS -m RN52 -f 0x50000 -S 0xc0000
  DEVICE_TITLE := Planex MZK-W300NH2
endef
TARGET_DEVICES += mzk-w300nh2


define LegacyDevice/NW718
  DEVICE_TITLE := Netcore NW718
endef
LEGACY_DEVICES += NW718


define LegacyDevice/PX-4885
  DEVICE_TITLE := 7Links PX-4885
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-dwc2 kmod-usb2 kmod-usb-ohci kmod-ledtrig-usbdev kmod-leds-gpio
endef
LEGACY_DEVICES += PX-4885


define LegacyDevice/W150M
  DEVICE_TITLE := Tenda W150M
endef
LEGACY_DEVICES += W150M


define LegacyDevice/W306R_V20
  DEVICE_TITLE := Tenda W306R V2.0
endef
LEGACY_DEVICES += W306R_V20


define LegacyDevice/WL-341V3
  DEVICE_TITLE := Sitecom WL-341 v3
endef
LEGACY_DEVICES += WL-341V3


define LegacyDevice/WNCE2001
  DEVICE_TITLE := Netgear WNCE2001
endef
LEGACY_DEVICES += WNCE2001


define LegacyDevice/WR512-3GN
  DEVICE_TITLE := WR512-3GN-like router
endef
LEGACY_DEVICES += WR512-3GN


define LegacyDevice/WT1520
  DEVICE_TITLE := Nexx WT1520
endef
LEGACY_DEVICES += WT1520


define LegacyDevice/X5
  DEVICE_TITLE := Poray X5/X6
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-netdev kmod-ledtrig-timer
endef
LEGACY_DEVICES += X5


define LegacyDevice/X8
  DEVICE_TITLE := Poray X8
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-netdev kmod-ledtrig-timer
endef
LEGACY_DEVICES += X8


define LegacyDevice/WHRG300N
  DEVICE_TITLE := Buffalo WHR-G300N
endef
LEGACY_DEVICES += WHRG300N

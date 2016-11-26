define Build/tplink-fw
	mktplinkfw2 -c -B $(BOARD_ID) -s \
		-k $@ -o $@.new
	mv $@.new $@
endef

define Build/mktplinkfw2
	mktplinkfw2 -B $(BOARD_ID) -s -a 0x4 -j \
		-k $(IMAGE_KERNEL) -r $(IMAGE_ROOTFS) \
		-o $@
endef
DEVICE_VARS += BOARD_ID

define Device/lantiqTpLink
  KERNEL := kernel-bin | append-dtb | lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | tplink-fw
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := mktplinkfw2 | append-metadata | check-size $$$$(IMAGE_SIZE)
endef

define Device/TDW8970
  $(Device/lantiqTpLink)
  DEVICE_PROFILE := TDW8970
  BOARD_ID := TD-W8970v1
  IMAGE_SIZE := 7680k
  DEVICE_TITLE := TP-LINK TD-W8970
  DEVICE_PACKAGES:= kmod-ath9k wpad-mini kmod-usb-dwc2 kmod-usb-ledtrig-usbport
endef

define Device/TDW8980
  $(Device/lantiqTpLink)
  DEVICE_PROFILE := TDW8980
  BOARD_ID := TD-W8980v1
  IMAGE_SIZE := 7680k
  DEVICE_TITLE := TP-LINK TD-W8980
  DEVICE_PACKAGES:= kmod-ath9k kmod-owl-loader wpad-mini kmod-usb-dwc2 kmod-usb-ledtrig-usbport
endef

define Device/VR200v
  $(Device/lantiqTpLink)
  DEVICE_PROFILE := VR200v
  BOARD_ID := ArcherVR200V
  IMAGE_SIZE := 15808k
  DEVICE_TITLE := TP-LINK Archer VR200v
  DEVICE_PACKAGES:= kmod-usb-dwc2 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += TDW8970 TDW8980 VR200v


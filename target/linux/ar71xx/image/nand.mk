define Device/c-60
  DEVICE_TITLE := AirTight C-60
  DEVICE_PACKAGES := kmod-spi-gpio kmod-usb-core kmod-usb2 kmod-ath9k
  BOARDNAME = C-60
  BLOCKSIZE := 64k
  KERNEL_SIZE = 3648k
  IMAGE_SIZE = 32m
  IMAGES := sysupgrade.tar
  MTDPARTS = spi0.0:256k(u-boot)ro,128k(u-boot-env)ro,3648k(kernel),64k(art)ro;ar934x-nfc:32m(ubi)
  KERNEL := kernel-bin | patch-cmdline | lzma | uImage lzma
  IMAGE/sysupgrade.tar := sysupgrade-tar
endef

TARGET_DEVICES += c-60

define Device/domywifi-dw33d
  DEVICE_TITLE := DomyWifi DW33D
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage kmod-usb-ledtrig-usbport kmod-ath10k ath10k-firmware-qca988x
  BOARDNAME = DW33D
  IMAGE_SIZE = 16000k
  CONSOLE = ttyS0,115200
  MTDPARTS = spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,14528k(rootfs),1472k(kernel),64k(art)ro,16000k@0x50000(firmware);ar934x-nfc:96m(rootfs_data),32m(backup)ro
  IMAGE/sysupgrade.bin = append-rootfs | pad-rootfs | pad-to 14528k | append-kernel | check-size $$$$(IMAGE_SIZE)
endef
TARGET_DEVICES += domywifi-dw33d

define Build/MerakiNAND
	-$(STAGING_DIR_HOST)/bin/mkmerakifw \
		-B $(BOARDNAME) -s \
		-i $@ \
		-o $@.new
	@mv $@.new $@
endef

define Device/mr18
  DEVICE_TITLE := Meraki MR18
  DEVICE_PACKAGES := kmod-spi-gpio kmod-ath9k
  BOARDNAME = MR18
  BLOCKSIZE := 64k
  CONSOLE = ttyS0,115200
  MTDPARTS = ar934x-nfc:512k(nandloader)ro,8M(kernel),8M(recovery),113664k(ubi),128k@130944k(odm-caldata)ro
  IMAGES := sysupgrade.tar
  KERNEL := kernel-bin | patch-cmdline | MerakiNAND
  KERNEL_INITRAMFS := kernel-bin | patch-cmdline | MerakiNAND
  IMAGE/sysupgrade.tar := sysupgrade-tar
endef
TARGET_DEVICES += mr18

define Build/MerakiNAND-old
	-$(STAGING_DIR_HOST)/bin/mkmerakifw-old \
		-B $(BOARDNAME) -s \
		-i $@ \
		-o $@.new
	@mv $@.new $@
endef

define Device/z1
  DEVICE_TITLE := Meraki Z1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport kmod-spi-gpio kmod-ath9k kmod-owl-loader
  BOARDNAME = Z1
  BLOCKSIZE := 64k
  CONSOLE = ttyS0,115200
  MTDPARTS = ar934x-nfc:128K(loader1)ro,8064K(kernel),128K(loader2)ro,8064K(recovery),114560K(ubi),128K(origcaldata)ro
  IMAGES := sysupgrade.tar
  KERNEL := kernel-bin | patch-cmdline | MerakiNAND-old
  KERNEL_INITRAMFS := kernel-bin | patch-cmdline | MerakiNAND-old
  IMAGE/sysupgrade.tar := sysupgrade-tar
endef

TARGET_DEVICES += z1

define LegacyDevice/R6100
  DEVICE_TITLE := NETGEAR R6100
  DEVICE_PACKAGES := kmod-ath10k ath10k-firmware-qca988x kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += R6100

define LegacyDevice/WNDR3700V4
  DEVICE_TITLE := NETGEAR WNDR3700v4
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += WNDR3700V4

define LegacyDevice/WNDR4300V1
  DEVICE_TITLE := NETGEAR WNDR4300v1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += WNDR4300V1

define LegacyDevice/NBG6716
  DEVICE_TITLE := Zyxel NBG 6716
  DEVICE_PACKAGES := kmod-rtc-pcf8563 kmod-ath10k ath10k-firmware-qca988x
endef
LEGACY_DEVICES += NBG6716

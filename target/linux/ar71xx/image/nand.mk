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
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev kmod-spi-gpio kmod-ath9k kmod-owl-loader
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
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += R6100

define LegacyDevice/WNDR3700V4
  DEVICE_TITLE := NETGEAR WNDR3700v4
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += WNDR3700V4

define LegacyDevice/WNDR4300V1
  DEVICE_TITLE := NETGEAR WNDR4300v1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += WNDR4300V1

define LegacyDevice/NBG6716
  DEVICE_TITLE := Zyxel NBG 6716
  DEVICE_PACKAGES := kmod-rtc-pcf8563 kmod-ath10k ath10k-firmware-qca988x
endef
LEGACY_DEVICES += NBG6716

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
  IMAGE/sysupgrade.tar := sysupgrade-nand
endef
TARGET_DEVICES += mr18

define LegacyDevice/R6100
	DEVICE_TITLE := NETGEAR R6100
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += R6100

define LegacyDevice/WNDR4300
	DEVICE_TITLE := NETGEAR WNDR3700v4/WNDR4300
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += WNDR4300

define LegacyDevice/NBG6716
	DEVICE_TITLE := Zyxel NBG 6716
	DEVICE_PACKAGES := kmod-rtc-pcf8563 kmod-ath10k
endef
LEGACY_DEVICES += NBG6716

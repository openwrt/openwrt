define Device/8devices_mango-dvk
	$(call Device/FitImageLzma)
	DEVICE_VENDOR := 8devices
	DEVICE_MODEL := Mango-DVK
	IMAGE_SIZE := 27776k
	BLOCKSIZE := 64k
	SOC := ipq6010
	SUPPORTED_DEVICES += 8devices,mango
	IMAGE/sysupgrade.bin := append-kernel | pad-to 64k | append-rootfs | pad-rootfs | check-size | append-metadata
	DEVICE_PACKAGES := ipq-wifi-8devices_mango
endef
TARGET_DEVICES += 8devices_mango-dvk

define Device/linksys_mr7350
	$(call Device/FitImage)
	DEVICE_VENDOR := Linksys
	DEVICE_MODEL := MR7350
	SOC := ipq6000
	NAND_SIZE := 256m
	KERNEL_SIZE := 8192k
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	IMAGE_SIZE := 75776k
	IMAGES += factory.bin
	IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | \
		append-ubi | linksys-image type=MR7350
	DEVICE_PACKAGES := ipq-wifi-linksys_mr7350 \
		kmod-leds-pca963x kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += linksys_mr7350

define Device/netgear_wax214
       $(call Device/FitImage)
       $(call Device/UbiFit)
       DEVICE_VENDOR := Netgear
       DEVICE_MODEL := WAX214
       BLOCKSIZE := 128k
       PAGESIZE := 2048
       DEVICE_DTS_CONFIG := config@cp03-c1
       SOC := ipq6010
       DEVICE_PACKAGES := ipq-wifi-netgear_wax214
endef
TARGET_DEVICES += netgear_wax214

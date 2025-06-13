DEVICE_VARS += TPLINK_SUPPORT_STRING

define Build/wax610-netgear-tar
	mkdir $@.tmp
	mv $@ $@.tmp/nand-ipq6018-apps.img
	md5sum $@.tmp/nand-ipq6018-apps.img | cut -c 1-32 > $@.tmp/nand-ipq6018-apps.md5sum
	echo "WAX610" > $@.tmp/metadata.txt
	echo "WAX610-610Y_V99.9.9.9" > $@.tmp/version
 	tar -C $@.tmp/ -cf $@ .
	rm -rf $@.tmp
endef

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

define Device/alfa-network_ap120c-ax
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := ALFA Network
	DEVICE_MODEL := AP120C-AX
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	SOC := ipq6000
	DEVICE_PACKAGES := ipq-wifi-alfa-network_ap120c-ax
endef
TARGET_DEVICES += alfa-network_ap120c-ax

define Device/cambiumnetworks_xe3-4
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Cambium Networks
	DEVICE_MODEL := XE3-4
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@cp01-c3-xv3-4
	SOC := ipq6010
	DEVICE_PACKAGES := ipq-wifi-cambiumnetworks_xe34 ath11k-firmware-qcn9074 kmod-ath11k-pci
endef
TARGET_DEVICES += cambiumnetworks_xe3-4

define Device/edgecore_eap101
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Edgecore
	DEVICE_MODEL := EAP101
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@cp01-c1
	SOC := ipq6018
	DEVICE_PACKAGES := ipq-wifi-edgecore_eap101
	IMAGE/factory.ubi := append-ubi | qsdk-ipq-factory-nand
endef
TARGET_DEVICES += edgecore_eap101

define Device/glinet_gl-common
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := GL.iNet
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@cp03-c1
	SOC := ipq6000
	IMAGES += factory.bin
	IMAGE/factory.bin := append-ubi | append-gl-metadata
endef

define Device/glinet_gl-ax1800
	$(call Device/glinet_gl-common)
	DEVICE_MODEL := GL-AX1800
	DEVICE_PACKAGES := ipq-wifi-glinet_gl-ax1800
	SUPPORTED_DEVICES += glinet,ax1800
endef
TARGET_DEVICES += glinet_gl-ax1800

define Device/glinet_gl-axt1800
	$(call Device/glinet_gl-common)
	DEVICE_MODEL := GL-AXT1800
	DEVICE_PACKAGES := ipq-wifi-glinet_gl-axt1800 kmod-hwmon-pwmfan
	SUPPORTED_DEVICES += glinet,axt1800
endef
TARGET_DEVICES += glinet_gl-axt1800

define Device/linksys_mr
	$(call Device/FitImage)
	DEVICE_VENDOR := Linksys
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	KERNEL_SIZE := 8192k
	IMAGES += factory.bin
	IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | linksys-image type=$$$$(DEVICE_MODEL)
	DEVICE_PACKAGE := kmod-usb-ledtrig-usbport
endef

define Device/linksys_mr7350
	$(call Device/linksys_mr)
	DEVICE_MODEL := MR7350
	NAND_SIZE := 256m
	IMAGE_SIZE := 75776k
	SOC := ipq6000
	DEVICE_PACKAGES += ipq-wifi-linksys_mr7350 kmod-leds-pca963x
endef
TARGET_DEVICES += linksys_mr7350

define Device/linksys_mr7500
	$(call Device/linksys_mr)
	DEVICE_MODEL := MR7500
	SOC := ipq6018
	NAND_SIZE := 512m
	IMAGE_SIZE := 147456k
	DEVICE_PACKAGES += ipq-wifi-linksys_mr7500 \
		ath11k-firmware-qcn9074 kmod-ath11k-pci \
		kmod-leds-pwm kmod-phy-aquantia
endef
TARGET_DEVICES += linksys_mr7500

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

define Device/netgear_wax610-common
	$(call Device/FitImage)
	DEVICE_VENDOR := Netgear
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@cp03-c1
	SOC := ipq6010
	KERNEL_IN_UBI := 1
	IMAGES += ui-factory.tar
	IMAGE/ui-factory.tar := append-ubi | qsdk-ipq-factory-nand | pad-to 4096 | wax610-netgear-tar
endef

define Device/netgear_wax610
	$(Device/netgear_wax610-common)
	DEVICE_MODEL := WAX610
	DEVICE_PACKAGES := ipq-wifi-netgear_wax610
endef
TARGET_DEVICES += netgear_wax610

define Device/netgear_wax610y
	$(Device/netgear_wax610-common)
	DEVICE_MODEL := WAX610Y
	DEVICE_PACKAGES := ipq-wifi-netgear_wax610y
endef
TARGET_DEVICES += netgear_wax610y

define Device/qihoo_360v6
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Qihoo
	DEVICE_MODEL := 360V6
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	SOC := ipq6000
	DEVICE_DTS_CONFIG := config@cp03-c1
	DEVICE_PACKAGES := ipq-wifi-qihoo_360v6
endef
TARGET_DEVICES += qihoo_360v6

define Device/tplink_eap610-outdoor
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := TP-Link
	DEVICE_MODEL := EAP610-Outdoor
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	SOC := ipq6018
	DEVICE_PACKAGES := ipq-wifi-tplink_eap610-outdoor
	IMAGES += web-ui-factory.bin
	IMAGE/web-ui-factory.bin := append-ubi | tplink-image-2022
	TPLINK_SUPPORT_STRING := SupportList:\r\n \
		EAP610-Outdoor(TP-Link|UN|AX1800-D):1.0\r\n \
		EAP610-Outdoor(TP-Link|JP|AX1800-D):1.0\r\n \
		EAP610-Outdoor(TP-Link|CA|AX1800-D):1.0
endef
TARGET_DEVICES += tplink_eap610-outdoor

define Device/tplink_eap623od-hd-v1
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := TP-Link
	DEVICE_MODEL := EAP623-Outdoor HD
	DEVICE_VARIANT := v1
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	SOC := ipq6018
	DEVICE_PACKAGES := ipq-wifi-tplink_eap623od-hd-v1 kmod-phy-realtek
	IMAGES += web-ui-factory.bin
	IMAGE/web-ui-factory.bin := append-ubi | tplink-image-2022
	TPLINK_SUPPORT_STRING := SupportList:\r\nEAP623-Outdoor HD(TP-Link|UN|AX1800-D):1.0\r\n
endef
TARGET_DEVICES += tplink_eap623od-hd-v1

define Device/tplink_eap625-outdoor-hd-v1
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := TP-Link
	DEVICE_MODEL := EAP625-Outdoor HD v1 and v1.6
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	SOC := ipq6018
	DEVICE_PACKAGES := ipq-wifi-tplink_eap625-outdoor-hd-v1
	IMAGES += web-ui-factory.bin
	IMAGE/web-ui-factory.bin := append-ubi | tplink-image-2022
	TPLINK_SUPPORT_STRING := SupportList:\r\n \
		EAP625-Outdoor HD(TP-Link|UN|AX1800-D):1.0\r\n \
		EAP625-Outdoor HD(TP-Link|CA|AX1800-D):1.0\r\n \
		EAP625-Outdoor HD(TP-Link|AU|AX1800-D):1.0\r\n \
		EAP625-Outdoor HD(TP-Link|KR|AX1800-D):1.0

endef
TARGET_DEVICES += tplink_eap625-outdoor-hd-v1

define Device/yuncore_fap650
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Yuncore
	DEVICE_MODEL := FAP650
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@cp03-c1
	SOC := ipq6018
	DEVICE_PACKAGES := ipq-wifi-yuncore_fap650
	IMAGES := factory.ubi factory.ubin sysupgrade.bin
	IMAGE/factory.ubin := append-ubi | qsdk-ipq-factory-nand
endef
TARGET_DEVICES += yuncore_fap650

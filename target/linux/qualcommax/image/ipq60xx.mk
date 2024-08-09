define Device/wallys_dr6018-v4
        $(call Device/FitImage)
        $(call Device/UbiFit)
        DEVICE_VENDOR := Wallys
        DEVICE_MODEL := DR6018-V4
        BLOCKSIZE := 128k
        PAGESIZE := 2048
        SOC := ipq6010
        DEVICE_DTS_CONFIG := config@cp01-c4
        IMAGES := sysupgrade.tar nand-factory.bin
        IMAGE/sysupgrade.tar := sysupgrade-tar | append-metadata
        IMAGE/nand-factory.bin := append-ubi | qsdk-ipq-factory-nand
        SUPPORTED_DEVICES := wallys,dr6018
        DEVICE_PACKAGES := ath11k-wifi-wallys-dr6018-v4 uboot-envtools -kmod-usb-dwc3-of-simple kmod-usb-dwc3-qcom kmod-usb3 kmod-usb2

endef
TARGET_DEVICES += wallys_dr6018-v4

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


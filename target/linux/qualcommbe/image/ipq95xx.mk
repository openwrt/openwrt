DTS_DIR := $(DTS_DIR)/qcom
DEVICE_VARS += TPLINK_SUPPORT_STRING

define Device/8devices_kiwi-dvk
	$(call Device/FitImage)
	$(call Device/EmmcImage)
	DEVICE_VENDOR := 8devices
	DEVICE_MODEL := Kiwi-DVK
	DEVICE_DTS_CONFIG := config@8dev-kiwi
	SOC := ipq9570
	DEVICE_PACKAGES := kmod-ath12k ath12k-firmware-qcn9274 ipq-wifi-8devices_kiwi f2fsck mkf2fs kmod-sfp kmod-phy-maxlinear
	IMAGE/factory.bin := qsdk-ipq-factory-nor
endef
TARGET_DEVICES += 8devices_kiwi-dvk

define Device/qcom_rdp433
	$(call Device/FitImageLzma)
	DEVICE_VENDOR := Qualcomm Technologies, Inc.
	DEVICE_MODEL := RDP433
	DEVICE_VARIANT := AP-AL02-C4
	BOARD_NAME := ap-al02.1-c4
	DEVICE_DTS_CONFIG := config@rdp433
	DEVICE_DTS_DIR := $(DTS_DIR)
	SOC := ipq9574
	KERNEL_INSTALL := 1
	KERNEL_SIZE := 6096k
	IMAGE_SIZE := 25344k
	IMAGE/sysupgrade.bin := append-kernel | pad-to 64k | append-rootfs | pad-rootfs | check-size | append-metadata
endef
TARGET_DEVICES += qcom_rdp433

define Device/tplink_archer-be800-common
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := TP-Link
	DEVICE_MODEL := BE800
	DEVICE_DTS_CONFIG := config@al02-c4
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	IMAGE_SIZE := 51200k
	UBINIZE_OPTS := -E 5
	UBIFS_OPTS := -m 2048 -e 126976 -c 4096
	KERNEL_INSTALL := 1
	KERNEL_SIZE := 6144k
	SOC := ipq9574
	DEVICE_PACKAGES := kmod-ath12k ipq-wifi-tplink_archer-be800 ipq-wifi-firmware-tplink_archer-be800 f2fsck mkf2fs
	IMAGES += web-ui-factory.bin
	IMAGE/web-ui-factory.bin := append-ubi | tplink-image-2023
	TPLINK_SUPPORT_STRING := SupportList:\n$\
		{product_name:Archer BE800,product_ver:1.0.0,special_id:55530000}\n$\
		{product_name:Archer BE800,product_ver:1.0.0,special_id:43410000}\n$\
		{product_name:Archer BE800,product_ver:1.0.0,special_id:45550000}\n$\
		{product_name:Archer BE800,product_ver:1.0.0,special_id:484B0000}\n$\
		{product_name:Archer BE800,product_ver:1.0.0,special_id:41530000}\n$\
		{product_name:Archer BE800,product_ver:1.0.0,special_id:41550000}\n$\
		{product_name:Archer BE800,product_ver:1.0.0,special_id:53470000}\n$\
		{product_name:Archer BE800,product_ver:1.20,special_id:55530000}\n$\
		{product_name:Archer BE800,product_ver:1.20,special_id:45550000}\n$\
		{product_name:Archer BE800,product_ver:1.20,special_id:43410000}\n$\
		{product_name:Archer BE800,product_ver:1.20,special_id:41530000}\n$\
		{product_name:Archer BE800,product_ver:1.20,special_id:41550000}\n$\
		{product_name:Archer BE800,product_ver:1.20,special_id:53470000}\n
endef

define Device/tplink_archer-be800
	$(call Device/tplink_archer-be800-common)
	DEVICE_VARIANT := v1
	SUPPORTED_DEVICES += tplink,archer-be800-sfp
endef
TARGET_DEVICES += tplink_archer-be800

define Device/tplink_archer-be800-sfp
	$(call Device/tplink_archer-be800-common)
	DEVICE_VARIANT := v1-sfp
	SUPPORTED_DEVICES += tplink,archer-be800
endef
TARGET_DEVICES += tplink_archer-be800-sfp
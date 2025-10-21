DTS_DIR := $(DTS_DIR)/qcom
DEVICE_VARS += TPLINK_SUPPORT_STRING

define Device/8devices_kiwi-dvk
	$(call Device/FitImage)
	$(call Device/EmmcImage)
	DEVICE_VENDOR := 8devices
	DEVICE_MODEL := Kiwi-DVK
	DEVICE_DTS_CONFIG := config@8dev-kiwi
	SOC := ipq9570
	DEVICE_PACKAGES := kmod-ath12k ath12k-firmware-qcn9274 \
		ipq-wifi-8devices_kiwi f2fsck mkf2fs kmod-sfp \
		kmod-phy-maxlinear kmod-phy-realtek rtl826x-firmware
	IMAGE/factory.bin := qsdk-ipq-factory-nor
endef
TARGET_DEVICES += 8devices_kiwi-dvk

define Device/askey_sbe1v1k
	$(call Device/FitImage)
	$(call Device/EmmcImage)
	DEVICE_VENDOR := Askey
	DEVICE_MODEL := SBE1V1K
	DEVICE_ALT0_VENDOR := Askey
	DEVICE_ALT0_MODEL := RTQ7300T
	DEVICE_ALT1_VENDOR := Spectrum
	DEVICE_ALT1_MODEL := SBE1V1K
	DEVICE_DTS_CONFIG := config@rtq7300t-rev0
	KERNEL_LOADADDR := 0x42200000
	SOC := ipq9570
	DEVICE_PACKAGES := ath12k-firmware-qcn9274 f2fsck ipq-wifi-askey_sbe1v1k kmod-ath12k \
		kmod-hwmon-pwmfan kmod-phy-realtek mkf2fs rtl826x-firmware
endef
TARGET_DEVICES += askey_sbe1v1k

define Device/linksys_ln6001
	$(call Device/FitImage)
	DEVICE_VENDOR := Linksys
	DEVICE_MODEL := LN6001
	DEVICE_DTS := ipq9554-linksys-ln6001
	SOC := ipq9554
	DEVICE_PACKAGES += bluetooth-uart kmod-leds-pwm kmod-fs-f2fs mkf2fs f2fsck
	IMAGE/sysupgrade.bin/squashfs := append-rootfs | pad-to 64k | \
		check-size 128m | sysupgrade-tar rootfs=$$$$@ | append-metadata
endef
TARGET_DEVICES += linksys_ln6001

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
	DEVICE_MODEL := Archer BE800
	DEVICE_DTS_CONFIG := config@al02-c4
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	IMAGE_SIZE := 51200k
	UBINIZE_OPTS := -E 5
	KERNEL_INSTALL := 1
	KERNEL_SIZE := 6144k
	SOC := ipq9574
	DEVICE_PACKAGES := kmod-ath12k kmod-phy-aquantia ipq-wifi-tplink_archer-be800 ath12k-firmware-tplink_archer-be800 kmod-sfp
	IMAGES += web-ui-factory.bin
	IMAGE/web-ui-factory.bin := append-ubi | tplink-image-2023
	TPLINK_SUPPORT_STRING := SupportList:\r\n$\
		{product_name:Archer BE800,product_ver:1.0.0,special_id:55530000}\r\n$\
		{product_name:Archer BE800,product_ver:1.0.0,special_id:43410000}\r\n$\
		{product_name:Archer BE800,product_ver:1.0.0,special_id:45550000}\r\n$\
		{product_name:Archer BE800,product_ver:1.0.0,special_id:484B0000}\r\n$\
		{product_name:Archer BE800,product_ver:1.0.0,special_id:41530000}\r\n$\
		{product_name:Archer BE800,product_ver:1.0.0,special_id:41550000}\r\n$\
		{product_name:Archer BE800,product_ver:1.0.0,special_id:53470000}\r\n$\
		{product_name:Archer BE800,product_ver:1.20,special_id:55530000}\r\n$\
		{product_name:Archer BE800,product_ver:1.20,special_id:45550000}\r\n$\
		{product_name:Archer BE800,product_ver:1.20,special_id:43410000}\r\n$\
		{product_name:Archer BE800,product_ver:1.20,special_id:41530000}\r\n$\
		{product_name:Archer BE800,product_ver:1.20,special_id:41550000}\r\n$\
		{product_name:Archer BE800,product_ver:1.20,special_id:53470000}\r\n
endef

define Device/tplink_archer-be800
	$(call Device/tplink_archer-be800-common)
	DEVICE_VARIANT := RJ45
	SUPPORTED_DEVICES += tplink,archer-be800-sfp
endef
TARGET_DEVICES += tplink_archer-be800-rj45

define Device/tplink_archer-be800-sfp
	$(call Device/tplink_archer-be800-common)
	DEVICE_VARIANT := SFP
	SUPPORTED_DEVICES += tplink,archer-be800-rj45
endef
TARGET_DEVICES += tplink_archer-be800-sfp

DTS_DIR := $(DTS_DIR)/qcom

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
	KERNEL_LOADADDR := 0x42080000
	SOC := ipq9570
	DEVICE_PACKAGES := ath12k-firmware-qcn9274 f2fsck ipq-wifi-askey_sbe1v1k kmod-ath12k \
		kmod-hwmon-pwmfan kmod-phy-realtek mkf2fs rtl826x-firmware
endef
TARGET_DEVICES += askey_sbe1v1k

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

define Device/jiorouter_ax6000-jidu6j11
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := JioRouter
	DEVICE_MODEL := AX6000
	SOC := ipq9554
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_PACKAGES := kmod-ath11k-ahb kmod-ath11k kmod-ath11k-pci \
		ath11k-firmware-ipq9574 ath11k-firmware-qcn9074 \
		ipq-wifi-jiorouter_ax6000-jidu6j11 \
		uboot-envtools kmod-leds-gpio kmod-gpio-button-hotplug \
		kmod-qcom-ppe kmod-usb3 kmod-usb-dwc3 kmod-usb-dwc3-qcom \
		kmod-ledtrig-netdev
endef

define Device/jiorouter_ax6000-jidu6j11-6111
	$(call Device/jiorouter_ax6000-jidu6j11)
	DEVICE_VARIANT := JIDU6J11-6111
	DEVICE_DTS := ipq9554-jiorouter-ax6000-jidu6j11-6111
endef
TARGET_DEVICES += jiorouter_ax6000-jidu6j11-6111

define Device/jiorouter_ax6000-jidu6j11-6411
	$(call Device/jiorouter_ax6000-jidu6j11)
	DEVICE_VARIANT := JIDU6J11-6411
	DEVICE_DTS := ipq9554-jiorouter-ax6000-jidu6j11-6411
endef
TARGET_DEVICES += jiorouter_ax6000-jidu6j11-6411

define Device/jiorouter_ax6000-jidu6j11-6611
	$(call Device/jiorouter_ax6000-jidu6j11)
	DEVICE_VARIANT := JIDU6J11-6611
	DEVICE_DTS := ipq9554-jiorouter-ax6000-jidu6j11-6611
endef
TARGET_DEVICES += jiorouter_ax6000-jidu6j11-6611

define Device/jiorouter_ax6000-jidu6j11-6811
	$(call Device/jiorouter_ax6000-jidu6j11)
	DEVICE_VARIANT := JIDU6J11-6811
	DEVICE_DTS := ipq9554-jiorouter-ax6000-jidu6j11-6811
endef
TARGET_DEVICES += jiorouter_ax6000-jidu6j11-6811

define Device/jiorouter_ax6000-jidu6j11-6911
	$(call Device/jiorouter_ax6000-jidu6j11)
	DEVICE_VARIANT := JIDU6J11-6911
	DEVICE_DTS := ipq9554-jiorouter-ax6000-jidu6j11-6911
endef
TARGET_DEVICES += jiorouter_ax6000-jidu6j11-6911

define Device/qcom_rdp466
	$(call Device/FitImageLzma)
	KERNEL_LOADADDR := 0x80000000
	DEVICE_VENDOR := Qualcomm Technologies, Inc.
	DEVICE_MODEL := RDP466
	DEVICE_VARIANT := AP-MR02
	BOARD_NAME := ap-mr02
	DEVICE_DTS_CONFIG := config@rdp466
	SOC := ipq5424
	KERNEL_INSTALL := 1
	KERNEL_SIZE := 6096k
	IMAGE_SIZE := 25344k
	IMAGE/sysupgrade.bin := append-kernel | pad-to 64k | append-rootfs | pad-rootfs | check-size | append-metadata
endef
TARGET_DEVICES += qcom_rdp466

define Device/qcom_rdp487
	$(call Device/FitImageLzma)
	KERNEL_LOADADDR := 0x80000000
	DEVICE_VENDOR := Qualcomm Technologies, Inc.
	DEVICE_MODEL := RDP487
	DEVICE_VARIANT := AP-MR03
	BOARD_NAME := ap-mr02
	DEVICE_DTS_CONFIG := config@rdp487
	SOC := ipq5424
	KERNEL_INSTALL := 1
	KERNEL_SIZE := 6096k
	IMAGE_SIZE := 25344k
	IMAGE/sysupgrade.bin := append-kernel | pad-to 64k | append-rootfs | pad-rootfs | check-size | append-metadata
endef
TARGET_DEVICES += qcom_rdp487

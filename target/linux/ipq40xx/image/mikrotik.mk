define Device/mikrotik_nor
	DEVICE_VENDOR := MikroTik
	BLOCKSIZE := 64k
	IMAGE_SIZE := 16128k
	KERNEL_NAME := vmlinux
	KERNEL := kernel-bin | append-dtb-elf
	IMAGES = sysupgrade.bin
	IMAGE/sysupgrade.bin := append-kernel | yaffs-filesystem -L | \
		pad-to $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | \
		check-size | append-metadata
endef

define Device/mikrotik_nand
	DEVICE_VENDOR := MikroTik
	KERNEL_NAME := vmlinux
	KERNEL_INITRAMFS := kernel-bin | append-dtb-elf
	KERNEL := kernel-bin | append-dtb-elf | package-kernel-ubifs | \
		ubinize-kernel
	IMAGES := sysupgrade.bin
	IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef

define Device/mikrotik_cap-ac
	$(call Device/mikrotik_nor)
	DEVICE_MODEL := cAP ac
	SOC := qcom-ipq4018
	DEVICE_PACKAGES := -kmod-ath10k-ct kmod-ath10k-ct-smallbuffers -kmod-usb-dwc3-qcom -kmod-usb-dwc3 -kmod-usb3
endef
TARGET_DEVICES += mikrotik_cap-ac

define Device/mikrotik_hap-ac2
	$(call Device/mikrotik_nor)
	DEVICE_MODEL := hAP ac2
	SOC := qcom-ipq4018
	DEVICE_PACKAGES := -kmod-ath10k-ct kmod-ath10k-ct-smallbuffers
endef
TARGET_DEVICES += mikrotik_hap-ac2

define Device/mikrotik_hap-ac3
	$(call Device/mikrotik_nand)
	DEVICE_MODEL := hAP ac3
	SOC := qcom-ipq4019
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	KERNEL_UBIFS_OPTS = -m $$(PAGESIZE) -e 124KiB -c $$(PAGESIZE) -x none
	DEVICE_PACKAGES := kmod-ledtrig-gpio
endef
TARGET_DEVICES += mikrotik_hap-ac3

define Device/mikrotik_hap-ac3-lte6-kit
        $(call Device/mikrotik_nor)
        DEVICE_MODEL := hAP ac3 LTE6 kit
        SOC := qcom-ipq4019
        DEVICE_PACKAGES := kmod-ledtrig-gpio kmod-usb-acm kmod-usb-net-rndis
endef
TARGET_DEVICES += mikrotik_hap-ac3-lte6-kit

define Device/mikrotik_lhgg-60ad
	$(call Device/mikrotik_nor)
	DEVICE_MODEL := Wireless Wire Dish LHGG-60ad
	DEVICE_DTS := qcom-ipq4019-lhgg-60ad
	DEVICE_PACKAGES += -kmod-ath10k-ct -ath10k-firmware-qca4019-ct kmod-wil6210 -kmod-usb-dwc3-qcom -kmod-usb-dwc3 -kmod-usb3
endef
TARGET_DEVICES += mikrotik_lhgg-60ad

define Device/mikrotik_sxtsq-5-ac
	$(call Device/mikrotik_nor)
	DEVICE_MODEL := SXTsq 5 ac (RBSXTsqG-5acD)
	SOC := qcom-ipq4018
	DEVICE_PACKAGES := rssileds -kmod-usb-dwc3-qcom -kmod-usb-dwc3 -kmod-usb3
endef
TARGET_DEVICES += mikrotik_sxtsq-5-ac

define Device/mikrotik_wap-ac
	$(call Device/mikrotik_nor)
	DEVICE_MODEL := wAP ac
	SOC := qcom-ipq4018
	DEVICE_PACKAGES := -kmod-ath10k-ct kmod-ath10k-ct-smallbuffers -kmod-usb-dwc3-qcom -kmod-usb-dwc3 -kmod-usb3
endef
TARGET_DEVICES += mikrotik_wap-ac

define Device/mikrotik_wap-r-ac
	$(call Device/mikrotik_wap-ac)
	DEVICE_MODEL := wAP R ac
	DEVICE_PACKAGES := kmod-usb-net-qmi-wwan kmod-usb-serial-option uqmi \
		kmod-usb-acm kmod-usb-net-rndis
	DEVICE_DTS := qcom-ipq4018-wap-r-ac
endef
TARGET_DEVICES += mikrotik_wap-r-ac

define Device/mikrotik_wap-ac-lte
	$(call Device/mikrotik_wap-ac)
	DEVICE_MODEL := wAP ac LTE
	DEVICE_PACKAGES := kmod-usb-net-qmi-wwan kmod-usb-serial-option uqmi \
		kmod-usb-acm kmod-usb-net-rndis
	DEVICE_DTS := qcom-ipq4018-wap-ac-lte
	DEVICE_ALT0_VENDOR = Mikrotik
	DEVICE_ALT0_MODEL := wAP ac LTE6
endef
TARGET_DEVICES += mikrotik_wap-ac-lte

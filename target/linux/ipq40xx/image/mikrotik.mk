define Device/mikrotik_nor
	DEVICE_VENDOR := MikroTik
	BLOCKSIZE := 64k
	IMAGE_SIZE := 16128k
	KERNEL_NAME := vmlinux
	KERNEL := kernel-bin | append-dtb-elf
	IMAGES = sysupgrade.bin
	IMAGE/sysupgrade.bin := append-kernel | kernel2minor -s 1024 | \
		pad-to $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | \
		check-size | append-metadata
endef

define Device/mikrotik_7442a-lhg5acd
	$(call Device/mikrotik_nor)
	SOC := qcom-ipq4018
	DEVICE_PACKAGES := ipq-wifi-mikrotik_7442a-lhg5acd rssileds \
	-kmod-ath10k-ct kmod-ath10k \
	-ath10k-firmware-qca4019-ct ath10k-firmware-qca4019 \
	-kmod-usb-dwc3-qcom -swconfig \
	-kmod-usb3 -kmod-usb-dwc3
endef

define Device/mikrotik_hap-ac2
	$(call Device/mikrotik_nor)
	DEVICE_MODEL := hAP ac2
	SOC := qcom-ipq4018
	DEVICE_PACKAGES := ipq-wifi-mikrotik_hap-ac2 -kmod-ath10k-ct \
		kmod-ath10k-ct-smallbuffers
endef
TARGET_DEVICES += mikrotik_hap-ac2

define Device/mikrotik_ldf-5-ac
	$(call Device/mikrotik_7442a-lhg5acd)
	DEVICE_MODEL := LDF 5 ac (RBLDFG-5acD)
endef
TARGET_DEVICES += mikrotik_ldf-5-ac

define Device/mikrotik_lhg-5-ac
	$(call Device/mikrotik_7442a-lhg5acd)
	DEVICE_MODEL := LHG [XL] 5 ac (RBLHGG-5acD[-XL])
endef
TARGET_DEVICES += mikrotik_lhg-5-ac

define Device/mikrotik_sxtsq-5-ac
	$(call Device/mikrotik_7442a-lhg5acd)
	DEVICE_MODEL := SXTsq 5 ac (RBSXTsqG-5acD)
endef
TARGET_DEVICES += mikrotik_sxtsq-5-ac

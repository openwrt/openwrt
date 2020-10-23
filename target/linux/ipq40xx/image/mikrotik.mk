define Device/mikrotik_nor
	DEVICE_VENDOR := MikroTik
	BLOCKSIZE := 64k
	IMAGE_SIZE := 16128k
	KERNEL_NAME := vmlinux
	KERNEL := kernel-bin | append-dtb-elf
	IMAGES = sysupgrade.bin
	IMAGE/sysupgrade.bin := append-kernel | kernel2minor -s 1024 | \
		pad-to $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | \
		append-metadata | check-size
endef

define Device/mikrotik_hap-ac2
	$(call Device/mikrotik_nor)
	DEVICE_MODEL := hAP ac2
	SOC := qcom-ipq4018
	DEVICE_PACKAGES := ipq-wifi-mikrotik_hap-ac2 -kmod-ath10k-ct \
		kmod-ath10k-ct-smallbuffers
endef
TARGET_DEVICES += mikrotik_hap-ac2

define Device/mikrotik_sxtsq-5-ac
	$(call Device/mikrotik_nor)
	DEVICE_MODEL := SXTsq 5 ac (RBSXTsqG-5acD)
	SOC := qcom-ipq4018
	DEVICE_PACKAGES := ipq-wifi-mikrotik_sxtsq-5-ac rssileds
endef
TARGET_DEVICES += mikrotik_sxtsq-5-ac

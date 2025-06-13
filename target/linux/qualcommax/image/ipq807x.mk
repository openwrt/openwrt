DEVICE_VARS += NETGEAR_BOARD_ID NETGEAR_HW_ID TPLINK_SUPPORT_STRING

define Build/asus-fake-ramdisk
	rm -rf $(KDIR)/tmp/fakerd
	dd if=/dev/zero bs=32 count=1 > $(KDIR)/tmp/fakerd
	$(info KERNEL_INITRAMFS is $(KERNEL_INITRAMFS))
endef

define Build/asus-fake-rootfs
	$(eval comp=$(word 1,$(1)))
	$(eval filepath=$(word 2,$(1)))
	$(eval filecont=$(word 3,$(1)))
	rm -rf $(KDIR)/tmp/fakefs $(KDIR)/tmp/fakehsqs
	mkdir -p $(KDIR)/tmp/fakefs/$$(dirname $(filepath))
	echo '$(filecont)' > $(KDIR)/tmp/fakefs/$(filepath)
	$(STAGING_DIR_HOST)/bin/mksquashfs4 $(KDIR)/tmp/fakefs $(KDIR)/tmp/fakehsqs -comp $(comp) \
		-b 4096 -no-exports -no-sparse -no-xattrs -all-root -noappend \
		$(wordlist 4,$(words $(1)),$(1))
endef

define Build/asus-trx
	$(STAGING_DIR_HOST)/bin/asusuimage $(wordlist 1,$(words $(1)),$(1)) -i $@ -o $@.new
	mv $@.new $@
endef

define Build/wax6xx-netgear-tar
	mkdir $@.tmp
	mv $@ $@.tmp/nand-ipq807x-apps.img
	md5sum $@.tmp/nand-ipq807x-apps.img | cut -c 1-32 > $@.tmp/nand-ipq807x-apps.md5sum
	echo $(DEVICE_MODEL) > $@.tmp/metadata.txt
	echo $(DEVICE_MODEL)"_V99.9.9.9" > $@.tmp/version
	tar -C $@.tmp/ -cf $@ .
	rm -rf $@.tmp
endef

define Device/aliyun_ap8220
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Aliyun
	DEVICE_MODEL := AP8220
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@ac02
	SOC := ipq8071
	DEVICE_PACKAGES := ipq-wifi-aliyun_ap8220
endef
TARGET_DEVICES += aliyun_ap8220

define Device/arcadyan_aw1000
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Arcadyan
	DEVICE_MODEL := AW1000
	BLOCKSIZE := 256k
	PAGESIZE := 4096
	DEVICE_DTS_CONFIG := config@hk09
	SOC := ipq8072
	DEVICE_PACKAGES := ipq-wifi-arcadyan_aw1000 kmod-spi-gpio \
		kmod-gpio-nxp-74hc164 kmod-usb-serial-option uqmi
endef
TARGET_DEVICES += arcadyan_aw1000

define Device/asus_rt-ax89x
	DEVICE_VENDOR := Asus
	DEVICE_MODEL := RT-AX89X
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@hk01
	SOC := ipq8074
	DEVICE_PACKAGES := kmod-hwmon-gpiofan ipq-wifi-asus_rt-ax89x
	KERNEL_NAME := vmlinux
	KERNEL := kernel-bin | libdeflate-gzip
	KERNEL_IN_UBI := 1
	IMAGE/sysupgrade.bin/squashfs := \
		append-kernel | asus-fake-ramdisk |\
		multiImage gzip $$(KDIR)/tmp/fakerd $$(KDIR)/image-$$(DEVICE_DTS).dtb |\
		sysupgrade-tar kernel=$$$$@ | append-metadata
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	ARTIFACTS := initramfs-factory.trx initramfs-uImage.itb
	ARTIFACT/initramfs-uImage.itb := \
		append-image-stage initramfs-kernel.bin | fit gzip $$(KDIR)/image-$$(DEVICE_DTS).dtb
	ARTIFACT/initramfs-factory.trx := \
		append-image-stage initramfs-kernel.bin |\
		asus-fake-rootfs xz /lib/firmware/IPQ8074A/fw_version.txt "fake" -no-compression |\
		multiImage gzip $$(KDIR)/tmp/fakehsqs $$(KDIR)/image-$$(DEVICE_DTS).dtb |\
		asus-trx -v 2 -n RT-AX89U -b 388 -e 49000
endif
endef
TARGET_DEVICES += asus_rt-ax89x

define Device/buffalo_wxr-5950ax12
	$(call Device/FitImage)
	DEVICE_VENDOR := Buffalo
	DEVICE_MODEL := WXR-5950AX12
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@hk01
	SOC := ipq8074
	DEVICE_PACKAGES := ipq-wifi-buffalo_wxr-5950ax12
endef
TARGET_DEVICES += buffalo_wxr-5950ax12

define Device/cmcc_rm2-6
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := CMCC
	DEVICE_MODEL := RM2-6
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@ac02
	SOC := ipq8070
	IMAGES += factory.bin
	IMAGE/factory.bin := append-ubi | qsdk-ipq-factory-nand
	DEVICE_PACKAGES := ipq-wifi-cmcc_rm2-6 kmod-hwmon-gpiofan
endef
TARGET_DEVICES += cmcc_rm2-6

define Device/compex_wpq873
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Compex
	DEVICE_MODEL := WPQ873
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@hk09.wpq873
	SOC := ipq8072
	DEVICE_PACKAGES := ipq-wifi-compex_wpq873
	IMAGE/factory.ubi := append-ubi | qsdk-ipq-factory-nand
endef
TARGET_DEVICES += compex_wpq873

define Device/dynalink_dl-wrx36
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Dynalink
	DEVICE_MODEL := DL-WRX36
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@rt5010w-d350-rev0
	SOC := ipq8072
	DEVICE_PACKAGES := ipq-wifi-dynalink_dl-wrx36
endef
TARGET_DEVICES += dynalink_dl-wrx36

define Device/edgecore_eap102
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Edgecore
	DEVICE_MODEL := EAP102
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@ac02
	SOC := ipq8071
	DEVICE_PACKAGES := ipq-wifi-edgecore_eap102
	IMAGE/factory.ubi := append-ubi | qsdk-ipq-factory-nand
endef
TARGET_DEVICES += edgecore_eap102

define Device/edimax_cax1800
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Edimax
	DEVICE_MODEL := CAX1800
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@ac03
	SOC := ipq8070
	DEVICE_PACKAGES := ipq-wifi-edimax_cax1800
endef
TARGET_DEVICES += edimax_cax1800

define Device/linksys_homewrk
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Linksys
	DEVICE_MODEL := HomeWRK
	DEVICE_DTS_CONFIG := config@oak03
	BLOCKSIZE := 256k
	PAGESIZE := 4096
	IMAGE_SIZE := 475m
	NAND_SIZE := 1024m
	SOC := ipq8174
	DEVICE_PACKAGES += kmod-leds-pca963x ipq-wifi-linksys_homewrk
endef
TARGET_DEVICES += linksys_homewrk

define Device/linksys_mx
	$(call Device/FitImage)
	DEVICE_VENDOR := Linksys
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	KERNEL_SIZE := 6144k
	IMAGE_SIZE := 147456k
	NAND_SIZE := 512m
	SOC := ipq8072
	IMAGES += factory.bin
	IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | linksys-image type=$$$$(DEVICE_MODEL)
	DEVICE_PACKAGES := kmod-leds-pca963x
endef

define Device/linksys_mx4x00
	$(call Device/linksys_mx)
	SOC := ipq8174
	DEVICE_PACKAGES += ipq-wifi-linksys_mx4200
endef

define Device/linksys_mx4200v1
	$(call Device/linksys_mx4x00)
	DEVICE_MODEL := MX4200
	DEVICE_VARIANT := v1
	DEVICE_PACKAGES += kmod-hci-uart
endef
TARGET_DEVICES += linksys_mx4200v1

define Device/linksys_mx4200v2
	$(call Device/linksys_mx4200v1)
	DEVICE_VARIANT := v2
endef
TARGET_DEVICES += linksys_mx4200v2

define Device/linksys_mx4300
	$(call Device/linksys_mx4x00)
	DEVICE_MODEL := MX4300
	BLOCKSIZE := 256k
	PAGESIZE := 4096
	KERNEL_SIZE := 8192k
	IMAGE_SIZE := 171264k
	NAND_SIZE := 1024m
endef
TARGET_DEVICES += linksys_mx4300

define Device/linksys_mx5300
	$(call Device/linksys_mx)
	DEVICE_MODEL := MX5300
	DEVICE_PACKAGES += kmod-rtc-ds1307 ipq-wifi-linksys_mx5300 \
		kmod-ath10k-ct ath10k-firmware-qca9984-ct
endef
TARGET_DEVICES += linksys_mx5300

define Device/linksys_mx8500
	$(call Device/linksys_mx)
	DEVICE_MODEL := MX8500
	DEVICE_PACKAGES += ipq-wifi-linksys_mx8500 kmod-ath11k-pci \
		ath11k-firmware-qcn9074 kmod-hci-uart
endef
TARGET_DEVICES += linksys_mx8500

define Device/netgear_rax120v2
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Netgear
	DEVICE_MODEL := RAX120v2
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@hk01
	SOC := ipq8074
	KERNEL_SIZE := 29696k
	NETGEAR_BOARD_ID := RAX120
	NETGEAR_HW_ID := 29765589+0+512+1024+4x4+8x8
	DEVICE_PACKAGES := ipq-wifi-netgear_rax120v2 kmod-spi-gpio \
		kmod-spi-bitbang kmod-gpio-nxp-74hc164 kmod-hwmon-g762
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	IMAGES += web-ui-factory.img
	IMAGE/web-ui-factory.img := append-image initramfs-uImage.itb | \
		pad-offset $$$$(BLOCKSIZE) 64 | append-uImage-fakehdr filesystem | \
		netgear-dni
endif
	IMAGE/sysupgrade.bin := append-kernel | pad-offset $$$$(BLOCKSIZE) 64 | \
		append-uImage-fakehdr filesystem | sysupgrade-tar kernel=$$$$@ | \
		append-metadata
endef
TARGET_DEVICES += netgear_rax120v2

define Device/netgear_sxk80
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_PACKAGES += ipq-wifi-netgear_sxk80
	DEVICE_VENDOR := Netgear
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@hk01
	SOC := ipq8074
	KERNEL_SIZE := 6272k
	NETGEAR_HW_ID := 29766265+0+512+1024+4x4+4x4+4x4
endef

define Device/netgear_sxr80
	$(call Device/netgear_sxk80)
	DEVICE_MODEL := SXR80
	NETGEAR_BOARD_ID := SXR80
endef
TARGET_DEVICES += netgear_sxr80

define Device/netgear_sxs80
	$(call Device/netgear_sxk80)
	DEVICE_MODEL := SXS80
	NETGEAR_BOARD_ID := SXS80
endef
TARGET_DEVICES += netgear_sxs80

define Device/netgear_wax218
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Netgear
	DEVICE_MODEL := WAX218
	DEVICE_DTS_CONFIG := config@hk07
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	SOC := ipq8072
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	ARTIFACTS := web-ui-factory.fit
	ARTIFACT/web-ui-factory.fit := append-image initramfs-uImage.itb | \
		ubinize-kernel | qsdk-ipq-factory-nand
endif
	DEVICE_PACKAGES := kmod-spi-gpio kmod-spi-bitbang kmod-gpio-nxp-74hc164 \
		ipq-wifi-netgear_wax218
endef
TARGET_DEVICES += netgear_wax218

define Device/netgear_wax620
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Netgear
	DEVICE_MODEL := WAX620
	DEVICE_DTS_CONFIG := config@hk07
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	SOC := ipq8072
	IMAGES += ui-factory.tar
	IMAGE/ui-factory.tar := append-ubi | qsdk-ipq-factory-nand | pad-to 4096 | wax6xx-netgear-tar
	DEVICE_PACKAGES := kmod-spi-gpio kmod-gpio-nxp-74hc164 \
		ipq-wifi-netgear_wax620
endef
TARGET_DEVICES += netgear_wax620

define Device/netgear_wax630
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Netgear
	DEVICE_MODEL := WAX630
	DEVICE_DTS_CONFIG := config@hk01
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	SOC := ipq8074
	IMAGES += ui-factory.tar
	IMAGE/ui-factory.tar := append-ubi | qsdk-ipq-factory-nand | pad-to 4096 | wax6xx-netgear-tar
	DEVICE_PACKAGES := kmod-spi-gpio ipq-wifi-netgear_wax630
endef
TARGET_DEVICES += netgear_wax630

define Device/prpl_haze
	$(call Device/FitImage)
	$(call Device/EmmcImage)
	DEVICE_VENDOR := prpl Foundation
	DEVICE_MODEL := Haze
	DEVICE_DTS_CONFIG := config@hk09
	SOC := ipq8072
	DEVICE_PACKAGES := ath11k-firmware-qcn9074 ipq-wifi-prpl_haze kmod-ath11k-pci \
		kmod-fs-f2fs f2fs-tools kmod-leds-lp5562
endef
TARGET_DEVICES += prpl_haze

define Device/qnap_301w
	$(call Device/FitImage)
	$(call Device/EmmcImage)
	DEVICE_VENDOR := QNAP
	DEVICE_MODEL := 301w
	DEVICE_DTS_CONFIG := config@hk01
	KERNEL_SIZE := 16384k
	SOC := ipq8072
	DEVICE_PACKAGES := kmod-fs-f2fs f2fs-tools ipq-wifi-qnap_301w
endef
TARGET_DEVICES += qnap_301w

define Device/redmi_ax6
	$(call Device/xiaomi_ax3600)
	DEVICE_VENDOR := Redmi
	DEVICE_MODEL := AX6
	DEVICE_PACKAGES := ipq-wifi-redmi_ax6
endef
TARGET_DEVICES += redmi_ax6

define Device/spectrum_sax1v1k
	$(call Device/FitImage)
	$(call Device/EmmcImage)
	DEVICE_VENDOR := Spectrum
	DEVICE_MODEL := SAX1V1K
	DEVICE_DTS_CONFIG := config@rt5010w-d187-rev6
	SOC := ipq8072
	IMAGES := sysupgrade.bin
	DEVICE_PACKAGES := kmod-fs-f2fs f2fs-tools ipq-wifi-spectrum_sax1v1k
endef
TARGET_DEVICES += spectrum_sax1v1k

define Device/tcl_hh500v
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := TCL
	DEVICE_MODEL := HH500V
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@hk09
	SOC := ipq8072
	IMAGES += factory.bin
	IMAGE/factory.bin := append-ubi | qsdk-ipq-factory-nand
	DEVICE_PACKAGES := ipq-wifi-tcl_hh500v kmod-usb-serial-option \
		kmod-pinctrl-aw9523 kmod-mhi-pci-generic kmod-mhi-wwan-ctrl \
		kmod-mhi-wwan-mbim
endef
TARGET_DEVICES += tcl_hh500v

define Device/tplink_deco-x80-5g
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := TP-Link
	DEVICE_MODEL := Deco X80-5G
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@hk01.c5
	SOC := ipq8074
	DEVICE_PACKAGES := kmod-hwmon-gpiofan ipq-wifi-tplink_deco-x80-5g \
	 	 kmod-usb-serial-option kmod-usb-net-qmi-wwan
endef
TARGET_DEVICES += tplink_deco-x80-5g

define Device/tplink_eap620hd-v1
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := TP-Link
	DEVICE_MODEL := EAP620 HD
	DEVICE_VARIANT := v1
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	SOC := ipq8072
	DEVICE_PACKAGES := ipq-wifi-tplink_eap620hd-v1
	IMAGES += web-ui-factory.bin
	IMAGE/web-ui-factory.bin := append-ubi | tplink-image-2022
	TPLINK_SUPPORT_STRING := SupportList:\r\nEAP620 HD(TP-Link|UN|AX1800-D):1.0\r\n
endef
TARGET_DEVICES += tplink_eap620hd-v1

define Device/tplink_eap660hd-v1
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := TP-Link
	DEVICE_MODEL := EAP660 HD
	DEVICE_VARIANT := v1
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	SOC := ipq8072
	DEVICE_PACKAGES := ipq-wifi-tplink_eap660hd-v1
	IMAGES += web-ui-factory.bin
	IMAGE/web-ui-factory.bin := append-ubi | tplink-image-2022
	TPLINK_SUPPORT_STRING := SupportList:\r\nEAP660 HD(TP-Link|UN|AX3600-D):1.0\r\n
endef
TARGET_DEVICES += tplink_eap660hd-v1

define Device/xiaomi_ax3600
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Xiaomi
	DEVICE_MODEL := AX3600
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@ac04
	SOC := ipq8071
	KERNEL_SIZE := 36608k
	DEVICE_PACKAGES := ipq-wifi-xiaomi_ax3600 kmod-ath10k-ct-smallbuffers ath10k-firmware-qca9887-ct
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	ARTIFACTS := initramfs-factory.ubi
	ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-uImage.itb | ubinize-kernel
endif
endef
TARGET_DEVICES += xiaomi_ax3600

define Device/xiaomi_ax9000
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Xiaomi
	DEVICE_MODEL := AX9000
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@hk14
	SOC := ipq8072
	KERNEL_SIZE := 57344k
	DEVICE_PACKAGES := ipq-wifi-xiaomi_ax9000 kmod-ath11k-pci ath11k-firmware-qcn9074 \
		kmod-ath10k-ct ath10k-firmware-qca9887-ct
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	ARTIFACTS := initramfs-factory.ubi
	ARTIFACT/initramfs-factory.ubi := append-image-stage initramfs-uImage.itb | ubinize-kernel
endif
endef
TARGET_DEVICES += xiaomi_ax9000

define Device/yuncore_ax880
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Yuncore
	DEVICE_MODEL := AX880
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@hk09
	SOC := ipq8072
	DEVICE_PACKAGES := ipq-wifi-yuncore_ax880
	IMAGES += factory.bin
	IMAGE/factory.bin := append-ubi | qsdk-ipq-factory-nand
endef
TARGET_DEVICES += yuncore_ax880

define Device/zbtlink_zbt-z800ax
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Zbtlink
	DEVICE_MODEL := ZBT-Z800AX
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@hk09
	SOC := ipq8072
	DEVICE_PACKAGES := ipq-wifi-zbtlink_zbt-z800ax
	IMAGES += factory.bin
	IMAGE/factory.bin := append-ubi | qsdk-ipq-factory-nand
endef
TARGET_DEVICES += zbtlink_zbt-z800ax

define Device/zte_mf269
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := ZTE
	DEVICE_MODEL := MF269
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	DEVICE_DTS_CONFIG := config@ac04
	SOC := ipq8071
	KERNEL_SIZE := 53248k
	DEVICE_PACKAGES := ipq-wifi-zte_mf269
endef
TARGET_DEVICES += zte_mf269

define Device/zyxel_nbg7815
	$(call Device/FitImage)
	$(call Device/EmmcImage)
	DEVICE_VENDOR := ZYXEL
	DEVICE_MODEL := NBG7815
	DEVICE_DTS_CONFIG := config@nbg7815
	SOC := ipq8074
	DEVICE_PACKAGES := kmod-fs-f2fs f2fs-tools ipq-wifi-zyxel_nbg7815 kmod-ath11k-pci \
		kmod-hci-uart kmod-hwmon-tmp103
endef
TARGET_DEVICES += zyxel_nbg7815

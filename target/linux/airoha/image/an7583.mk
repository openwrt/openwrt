define Build/an7583-preloader
  cat $(STAGING_DIR_IMAGE)/an7583_$1-bl2.fip >> $@
endef

define Build/an7583-bl31-uboot
  cat $(STAGING_DIR_IMAGE)/an7583_$1-bl31-u-boot.fip >> $@
endef

define Device/FitImageLzma
  KERNEL_SUFFIX := -uImage.itb
  KERNEL = kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(DEVICE_DTS).dtb
  KERNEL_NAME := Image
endef

define Device/airoha_an7583-evb
  $(call Device/FitImageLzma)
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := AN7583 Evaluation Board (SNAND)
  DEVICE_PACKAGES := kmod-phy-aeonsemi-as21xxx kmod-leds-pwm \
	kmod-pwm-airoha kmod-input-gpio-keys-polled
  DEVICE_DTS := an7583-evb
  DEVICE_DTS_CONFIG := config@1
  IMAGE/sysupgrade.bin := append-kernel | pad-to 128k | append-rootfs | \
	pad-rootfs | append-metadata
  ARTIFACT/preloader.bin := an7583-preloader rfb
  ARTIFACT/bl31-uboot.fip := an7583-bl31-uboot rfb
  ARTIFACTS := preloader.bin bl31-uboot.fip
endef
TARGET_DEVICES += airoha_an7583-evb

define Device/airoha_an7583-evb-emmc
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := AN7583 Evaluation Board (EMMC)
  DEVICE_DTS := an7583-evb-emmc
  DEVICE_PACKAGES := kmod-phy-airoha-en8811h
  ARTIFACT/preloader.bin := an7583-preloader rfb
  ARTIFACT/bl31-uboot.fip := an7583-bl31-uboot rfb
  ARTIFACTS := preloader.bin bl31-uboot.fip
endef
TARGET_DEVICES += airoha_an7583-evb-emmc

define Device/nokia_xg-040g-mf
  $(call Device/FitImageLzma)
  DEVICE_VENDOR := Nokia
  DEVICE_MODEL := XG-040G-MF
  DEVICE_DTS := an7583-nokia_xg-040g-mf
  DEVICE_DTS_CONFIG := config@1
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  UBINIZE_OPTS := -E 5
  IMAGE_SIZE := 131968k
  KERNEL_SIZE := 8192k
  IMAGES += factory-kernel.bin factory-rootfs.bin
  IMAGE/factory-kernel.bin := append-kernel
  IMAGE/factory-rootfs.bin := append-ubi | check-size
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-phy-airoha-en8811h
endef
TARGET_DEVICES += nokia_xg-040g-mf

define Device/vsol_an7583-common
  DEVICE_VENDOR := VSOL
  DEVICE_DTS_CONFIG := config@1
  DEVICE_PACKAGES := fitblk kmod-i2c-an7581
  KERNEL_SUFFIX := -kernel.bin
  KERNEL := kernel-bin | lzma
  KERNEL_NAME := Image
  KERNEL_INITRAMFS = kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(DEVICE_DTS).dtb
  KERNEL_INITRAMFS_SUFFIX := -uImage.itb
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 32768k
  IMAGE/sysupgrade.bin = append-kernel | fit lzma $$(KDIR)/image-$$(DEVICE_DTS).dtb \
	external-static-with-rootfs | airoha-image-tclinux an7583_vsol | check-size | append-metadata
  ARTIFACT/tcboot.bin := airoha-image-tcboot an7583_vsol | check-size 512k
  ARTIFACT/tclinux.bin := append-image squashfs-sysupgrade.bin
  ARTIFACTS := tcboot.bin tclinux.bin
endef

define Device/vsol_v2901q-a
  $(call Device/vsol_an7583-common)
  DEVICE_MODEL := V2901Q-A
  DEVICE_DTS := an7583-vsol-v2901q-a
  DEVICE_PACKAGES += kmod-phy-airoha-en8811h
endef
TARGET_DEVICES += vsol_v2901q-a

define Device/vsol_v2902a-s
  $(call Device/vsol_an7583-common)
  DEVICE_MODEL := V2902A-S
  DEVICE_DTS := an7583-vsol-v2902a-s
  DEVICE_PACKAGES += kmod-sfp
endef
TARGET_DEVICES += vsol_v2902a-s

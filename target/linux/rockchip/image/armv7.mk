# SPDX-License-Identifier: GPL-2.0-only

RKBOOT_FDT_ADDR := 0x08300000
RKBOOT_TAGS_OFFSET := 0x7f00
ROOTDEV_OVERLAY_ALIGN := 64k

DEVICE_VARS += FLS_IC FLS_SENSOR

define Build/append-file
	cat "$(1)" >> "$@"
endef

define Build/mkenv
	printf 'mtdparts=spi-nand0:256K(env),1M@256K(idblock),1M(uboot),$(call toupper,$(IMAGE_SIZE))(ubi)\n' \
		| mkenvimage -p 0x00 -s 0x40000 -o $@ -
endef

define Build/rk-bootimg
	$(STAGING_DIR_HOST)/bin/mkrkboot \
		-k $@ \
		-d $(KDIR)/image-$(firstword $(DEVICE_DTS)).dtb \
		-a $(KERNEL_LOADADDR) \
		-f $(RKBOOT_FDT_ADDR) \
		-t $$(( $(KERNEL_LOADADDR) - $(RKBOOT_TAGS_OFFSET) )) \
		-o $@.new
	mv $@.new $@
endef

define Build/herospeed-fls
	dd if=$(IMAGE_ROOTFS) of=$@.system bs=$(ROOTDEV_OVERLAY_ALIGN) conv=sync
	dd if=/dev/zero bs=4k count=1 >> $@.system
	$(STAGING_DIR_HOST)/bin/mkhsfls \
		-i $(FLS_IC) -s $(FLS_SENSOR) \
		-e boot=$(IMAGE_KERNEL) \
		-e system=$@.system \
		-o $@.new
	mv $@.new $@
	rm -f $@.system
endef

define Device/rv1126
  SOC := rv1126
  IMAGES :=
  KERNEL_NAME := zImage
  KERNEL = kernel-bin | rk-bootimg
  KERNEL_INITRAMFS = kernel-bin | rk-bootimg
  KERNEL_LOADADDR := 0x02008000
endef

define Device/herospeed_rv1126-imx415
  $(Device/rv1126)
  DEVICE_VENDOR := Herospeed
  DEVICE_MODEL := RV1126 IMX415 Base
  DEVICE_DTS := rv1126-herospeed-imx415
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-rtc-pcf8563 kmod-video-core kmod-video-imx415 \
	kmod-video-rockchip-cif kmod-phy-rockchip-inno-csidphy \
	kmod-leds-pwm-flash kmod-crypto-hw-rockchip2 kmod-video-hantro \
	kmod-video-rockchip-isp2 kmod-video-rockchip-rkvenc video-support
  IMAGES := sysupgrade.bin factory-RV1126_IMX415.fls
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE/factory-RV1126_IMX415.fls := herospeed-fls
  FLS_IC := RV1126
  FLS_SENSOR := IMX415
endef
TARGET_DEVICES += herospeed_rv1126-imx415

define Device/luckfox_webbee
  DEVICE_VENDOR := Luckfox
  DEVICE_MODEL := WebBee
  DEVICE_DTS := rv1103-luckfox-webbee
  DEVICE_DTS_DIR := ../dts
  DEVICE_PACKAGES := kmod-button-hotplug kmod-input-adc-keys kmod-leds-gpio usbgadget-ncm
  KERNEL_NAME := zImage
  KERNEL_LOADADDR := 0x00008000
  KERNEL_IN_UBI := 1
  KERNEL = kernel-bin | fit none $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  SOC := rv1103
  UBOOT_DEVICE_NAME = luckfox-pico-mini-b-rv1103
  FILESYSTEMS := squashfs ubifs
  IMAGES := factory.bin sysupgrade.bin
  IMAGE/factory.bin := mkenv | \
	append-file $(STAGING_DIR_IMAGE)/$$(UBOOT_DEVICE_NAME)-idbloader.img | pad-to 1280k | \
	append-file $(STAGING_DIR_IMAGE)/$$(UBOOT_DEVICE_NAME)-u-boot.img | pad-to 2304k | \
	append-ubi | pad-to $$(PAGESIZE) | check-size
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE_SIZE := 128768k
endef
TARGET_DEVICES += luckfox_webbee

# SPDX-License-Identifier: GPL-2.0-only

RKBOOT_FDT_ADDR := 0x08300000
RKBOOT_TAGS_OFFSET := 0x7f00
ROOTDEV_OVERLAY_ALIGN := 64k

DEVICE_VARS += FLS_IC FLS_SENSOR

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
	kmod-iio-rockchip-saradc video-support
  IMAGES := sysupgrade.bin factory-RV1126_IMX415.fls
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE/factory-RV1126_IMX415.fls := herospeed-fls
  FLS_IC := RV1126
  FLS_SENSOR := IMX415
endef
TARGET_DEVICES += herospeed_rv1126-imx415

# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

# Pass MF283V_SIGNING_KEY=/absolute/path/to/key on the make command line to
# include the LK-authenticated boot.bin artifact.
# Set MF283V_STRICT_KEY_FINGERPRINT=1 to reject keys which do not match the
# Qualcomm test key accepted by the tested MF283V bootloader.
MF283V_BOOT_ARTIFACT := $(if $(strip $(MF283V_SIGNING_KEY)),boot.bin)
MF283V_STRICT_KEY_ARG := \
	$(if $(filter 1,$(MF283V_STRICT_KEY_FINGERPRINT)),--strict-key-fingerprint)

define Build/mf283v-mkboot
	$(TOPDIR)/target/linux/mdm9607/image/mf283v-mkboot.py \
		--kernel $@ \
		--dtb $(KDIR)/image-$(DEVICE_DTS).dtb \
		--key "$(MF283V_SIGNING_KEY)" \
		--openssl-bin "$(STAGING_DIR_HOST)/bin/openssl" \
		$(MF283V_STRICT_KEY_ARG) \
		--cmdline "$(CMDLINE)" \
		--output $@.new
	mv $@.new $@
endef

define Device/Default
	DEVICE_DTS_DIR := ../dts
	KERNEL_NAME := zImage
	KERNEL := kernel-bin
	KERNEL_INITRAMFS := kernel-bin
	KERNEL_INITRAMFS_SUFFIX := -zImage
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	SUBPAGESIZE := 2048
	IMAGE_SIZE := 324864k
	IMAGES := system.ubi
	IMAGE/system.ubi := append-ubi | check-size $$(IMAGE_SIZE)
endef

define Device/zte_mf283v
	DEVICE_VENDOR := ZTE
	DEVICE_MODEL := MF283V
	DEVICE_DTS := qcom-mdm9607-zte-mf283v
	CMDLINE := console=ttyMSM0,115200n8 loglevel=7 clk_ignore_unused \
		panic=0 ubi.block=0,rootfs root=/dev/ubiblock0_0 \
		rootfstype=squashfs rootwait ro
	ARTIFACTS := $(MF283V_BOOT_ARTIFACT)
	ARTIFACT/boot.bin := kernel-bin | mf283v-mkboot
	DEVICE_PACKAGES := mtd ubi-utils mf283v-reboot mf283v-wlan-data \
		kmod-rtl8192es \
		kmod-dsa-qca8k kmod-phy-qca83xx wpad-basic-mbedtls ethtool
endef
TARGET_DEVICES += zte_mf283v

$(eval $(call BuildImage))

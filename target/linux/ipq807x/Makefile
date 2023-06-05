include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARD:=ipq807x
BOARDNAME:=Qualcomm Atheros IPQ807x
FEATURES:=squashfs ramdisk fpu nand rtc emmc
KERNELNAME:=Image dtbs
CPU_TYPE:=cortex-a53
SUBTARGETS:=generic

KERNEL_PATCHVER:=5.15

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += \
	kmod-usb3 kmod-usb-dwc3 kmod-usb-dwc3-qcom \
	kmod-leds-gpio kmod-gpio-button-hotplug \
	kmod-phy-aquantia kmod-qca-nss-dp \
	ath11k-firmware-ipq8074 kmod-ath11k-ahb \
	wpad-basic-mbedtls uboot-envtools \
	e2fsprogs kmod-fs-ext4 losetup

$(eval $(call BuildTarget))
